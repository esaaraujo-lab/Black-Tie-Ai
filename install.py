#!/usr/bin/env python3
"""Black Tie AI Producer + Black Tie Live — bootstrap/build/install.

Uso:
  python install.py               # instala deps, compila e instala os plugins
  python install.py --clean       # apaga builds/ e dist/ antes de compilar
  python install.py --no-install  # só compila, deixa em ./dist

Rode pelo PowerShell (pode ser janela normal — o script eleva sozinho).
NÃO rode pelo IDLE.
"""
from __future__ import annotations
import argparse, ctypes, os, platform, re, shutil, subprocess, sys, time, traceback, urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent
JUCE = ROOT / "third_party" / "JUCE"
DIST = ROOT / "dist"
TOOLS = ROOT / "tools"
JUCE_TAG = "9.0.1"
PROJECTS = [
    (ROOT / "vst3", "ChatGPTAbletonAIProducer"),
    (ROOT / "performance", "BlackTieLive"),
]
CLOUD_MARKS = ("onedrive", "google drive", "drives compartilhados", "dropbox", "icloud")
LOCAL_HOME = Path(r"C:\Black_Tie_AI_Producer")
IN_IDLE = "idlelib" in sys.modules

def log(m): print(f"[Black Tie] {m}", flush=True)
def run(cmd, cwd=ROOT, check=True):
    cmd = [str(c) for c in cmd]; log("$ " + " ".join(cmd))
    return subprocess.run(cmd, cwd=str(cwd), check=check)
def command(name): return shutil.which(name)
def is_cloud(p: Path) -> bool:
    return any(m in str(p).lower() for m in CLOUD_MARKS)

# ------------------------------------------------------------------ Windows --

def is_admin() -> bool:
    try: return ctypes.windll.shell32.IsUserAnAdmin() != 0
    except Exception: return False

def localize_and_elevate():
    """Copia o projeto para pasta local e relança como admin de lá.
    Unidades de nuvem (G: do Drive, OneDrive) são INVISÍVEIS para processos
    elevados — causam o 'piscou e fechou'."""
    dest = LOCAL_HOME; i = 2
    while dest.exists() and (dest / "install.py").exists():
        dest = Path(f"{LOCAL_HOME}-{i}"); i += 1
    log(f"Copiando projeto para pasta local: {dest}  (pula JUCE/builds/.git)")
    shutil.copytree(ROOT, dest,
                    ignore=shutil.ignore_patterns("third_party", "build-*", "dist", "tools", ".git"))
    log("Copiado. Abrindo janela de ADMINISTRADOR a partir da pasta local...")
    rc = ctypes.windll.shell32.ShellExecuteW(
        None, "runas", sys.executable, f'"{dest}\\install.py"', None, 1)
    if rc <= 32:
        raise SystemExit("UAC recusado. Abra PowerShell como Administrador e rode:\n"
                         f'  cd "{dest}"\n  python install.py')
    log("Janela admin aberta — continue por lá. Pode fechar esta.")
    sys.exit(0)

def relaunch_as_admin():
    script = Path(__file__).resolve()
    args = " ".join(f'"{a}"' for a in sys.argv[1:])
    rc = ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable,
                                             f'"{script}" {args}', None, 1)
    if rc > 32:
        log("Relançado como Administrador — continue na nova janela.")
        sys.exit(0)
    raise SystemExit("Sem privilégios de administrador. Abra PowerShell como Administrador.")

def add_windows_paths():
    extra = []
    for base in [os.environ.get("LOCALAPPDATA"), os.environ.get("ProgramFiles"),
                 os.environ.get("ProgramFiles(x86)")]:
        if base:
            extra += [Path(base) / "CMake" / "bin", Path(base) / "Git" / "cmd"]
    extra += [Path(r"C:\Program Files\CMake\bin"), Path(r"C:\Program Files\Git\cmd")]
    for p in extra:
        if p.exists(): os.environ["PATH"] = str(p) + os.pathsep + os.environ.get("PATH", "")

def defender_exclude_windows():
    """Evita LNK1104 no link e WinError 32/267 na cópia: o Defender escaneia a
    DLL recém-criada e a trava por instantes. Excluir a pasta do projeto resolve.
    Falha silenciosa se o Defender não existir (servidor, etc.)."""
    try:
        subprocess.run(["powershell", "-NoProfile", "-Command",
                        f"Add-MpPreference -ExclusionPath '{ROOT}'"],
                       check=False, capture_output=True)
        log(f"Defender: exclusão solicitada para {ROOT}")
    except Exception:
        pass

def vs_with_vc_tools():
    candidates = [command("vswhere"),
                  Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"))
                      / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"]
    for vsw in candidates:
        if vsw and Path(vsw).exists():
            r = subprocess.run([str(vsw), "-latest", "-products", "*", "-requires",
                                "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                                "-property", "installationPath"],
                               capture_output=True, text=True)
            if r.returncode == 0 and r.stdout.strip():
                return Path(r.stdout.strip())
    return None

def download(url: str, dest: Path):
    dest.parent.mkdir(parents=True, exist_ok=True)
    log(f"Baixando {url}")
    urllib.request.urlretrieve(url, dest)

def winget_install(wid: str, override=None) -> bool:
    if not command("winget"):
        log("winget indisponível — usando download direto."); return False
    cmd = ["winget", "install", "--id", wid, "-e", "--silent",
           "--accept-source-agreements", "--accept-package-agreements"]
    if override: cmd += ["--override", override]
    try: run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        log(f"winget falhou (exit {e.returncode}) para {wid} — usando fallback."); return False
    add_windows_paths(); return True

def install_git_windows():
    exe = TOOLS / "Git-installer.exe"
    download("https://github.com/git-for-windows/git/releases/download/"
             "v2.46.0.windows.1/Git-2.46.0-64-bit.exe", exe)
    run([exe, "/VERYSILENT", "/NORESTART", "/SUPPRESSMSGBOXES"])
    add_windows_paths()
    if not command("git"): raise RuntimeError("Falha ao instalar o Git.")

def install_cmake_windows():
    msi = TOOLS / "cmake-setup.msi"
    download("https://github.com/Kitware/CMake/releases/download/"
             "v3.29.6/cmake-3.29.6-windows-x86_64.msi", msi)
    run(["msiexec", "/i", msi, "/qn", "/norestart", "ADD_CMAKE_TO_PATH=System"])
    add_windows_paths()
    if not command("cmake"):
        raise RuntimeError("CMake instalado mas não visível. Feche e reabra o "
                           "PowerShell como Administrador e rode de novo.")

def install_buildtools_windows():
    boot = TOOLS / "vs_BuildTools.exe"
    if not boot.exists():
        download("https://aka.ms/vs/17/release/vs_BuildTools.exe", boot)
    log("Instalando VS 2022 Build Tools (10-30 min, ~4 GB). NÃO feche a janela.")
    code = subprocess.run([str(boot), "--quiet", "--wait", "--norestart",
                           "--add", "Microsoft.VisualStudio.Workload.VCTools",
                           "--includeRecommended"], cwd=str(TOOLS)).returncode
    if code not in (0, 3010, 1641):  # 3010/1641 = sucesso com reboot pendente
        raise RuntimeError(f"vs_BuildTools falhou: exit {code} "
                           f"(0x{code & 0xFFFFFFFF:08X}). Logs em %TEMP%\\dd_*.log")
    if code != 0: log("Instalado. Há um reinício do Windows pendente (pode depois).")

def ensure_windows():
    add_windows_paths()
    if is_cloud(ROOT):
        log("PROJETO EM PASTA DE NUVEM DETECTADO — isso quebra o build.")
        log("(Unidades de nuvem são INVISÍVEIS para processos elevados/admin.)")
        localize_and_elevate()
        return
    if not is_admin():
        log("Sem privilégios de admin — elevando...")
        relaunch_as_admin()

    defender_exclude_windows()   # ANTES do build: evita LNK1104/cópia travada

    installs = {"git": ("Git.Git", install_git_windows),
                "cmake": ("Kitware.CMake", install_cmake_windows)}
    for exe, (wid, fallback) in installs.items():
        if command(exe): log(f"{exe}: OK"); continue
        if not winget_install(wid): fallback()
        if not command(exe):
            raise RuntimeError(f"{exe} não ficou disponível. Reabra o terminal e rode de novo.")

    vs = vs_with_vc_tools()
    if vs:
        log(f"MSVC C++ Build Tools encontradas: {vs} — pulando instalação.")
    else:
        install_buildtools_windows()
        vs = vs_with_vc_tools()
        if not vs:
            raise RuntimeError("Build Tools instaladas mas não detectadas. "
                               "Reabra o PowerShell e rode de novo.")

# ------------------------------------------------------------------- macOS ---

def ensure_macos():
    if subprocess.run(["xcode-select", "-p"], stdout=subprocess.DEVNULL,
                      stderr=subprocess.DEVNULL).returncode != 0:
        raise RuntimeError("Command Line Tools ausentes. Rode 'xcode-select --install', "
                           "aprove o diálogo da Apple e rode install.py de novo.")
    if not command("git") or not command("cmake"):
        brew = command("brew")
        if not brew:
            log("Instalando Homebrew (não interativo)...")
            run(["/bin/bash", "-c",
                 'NONINTERACTIVE=1 /bin/bash -c "$(curl -fsSL '
                 'https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'])
            for p in ("/opt/homebrew/bin", "/usr/local/bin"):
                os.environ["PATH"] = p + os.pathsep + os.environ.get("PATH", "")
            brew = command("brew")
        if not brew: raise RuntimeError("Homebrew não disponível após instalação.")
        for exe, formula in [("git", "git"), ("cmake", "cmake")]:
            if not command(exe): run([brew, "install", formula])

# ------------------------------------------------------------------- JUCE ----

def ensure_juce():
    JUCE.parent.mkdir(parents=True, exist_ok=True)
    if (JUCE / "CMakeLists.txt").exists():
        log(f"JUCE {JUCE_TAG} já presente — pulando clone."); return
    if not command("git"): raise RuntimeError("Git é necessário antes do bootstrap do JUCE.")
    run(["git", "clone", "--depth", "1", "--branch", JUCE_TAG,
         "https://github.com/juce-framework/JUCE.git", str(JUCE)])

# ----------------------------------------------------------- patches CMake ---

def fix_juce_cmake(src: Path):
    """JUCE exige compilador C. 'LANGUAGES CXX' -> 'LANGUAGES C CXX'."""
    patched = 0
    for cm in src.rglob("CMakeLists.txt"):
        try: text = cm.read_text(encoding="utf-8", errors="ignore")
        except OSError: continue
        new = re.sub(r"\bLANGUAGES\s+CXX\b", "LANGUAGES C CXX", text)
        new = re.sub(r"enable_language\s*\(\s*CXX\s*\)", "enable_language(C CXX)", new)
        if new != text:
            cm.write_text(new, encoding="utf-8")
            log(f"Patch automático: {cm.relative_to(ROOT)} — adicionado 'C' às LANGUAGES")
            patched += 1
    if patched: log(f"{patched} CMakeLists.txt corrigido(s) [LANGUAGES C CXX].")

def _insert_juce_header_calls(text: str) -> str:
    """Injeta juce_generate_juce_header(<Plugin>) após cada juce_add_plugin(...)."""
    out, search_from = text, 0
    while True:
        idx = out.find("juce_add_plugin", search_from)
        if idx == -1: break
        open_paren = out.find("(", idx)
        if open_paren == -1: break
        depth, i = 0, open_paren
        while i < len(out):
            c = out[i]
            if c == "(": depth += 1
            elif c == ")":
                depth -= 1
                if depth == 0: break
            i += 1
        if depth != 0: break
        inner = out[open_paren + 1:i]
        tokens = inner.replace("\n", " ").split()
        if not tokens: search_from = i + 1; continue
        name = tokens[0].strip()
        insertion = ("\n\n# Auto-patch: os fontes incluem <JuceHeader.h>; no fluxo CMake\n"
                     f"# esse header só existe se for gerado explicitamente.\n"
                     f"juce_generate_juce_header({name})\n")
        end = i + 1
        out = out[:end] + insertion + out[end:]
        search_from = end + len(insertion)
    return out

def fix_juce_header(src: Path):
    """Fontes incluem <JuceHeader.h> — sem juce_generate_juce_header(): C1083."""
    patched = 0
    for cm in src.rglob("CMakeLists.txt"):
        try: text = cm.read_text(encoding="utf-8", errors="ignore")
        except OSError: continue
        if "juce_generate_juce_header" in text: continue
        new = _insert_juce_header_calls(text)
        if new != text:
            cm.write_text(new, encoding="utf-8")
            log(f"Patch automático: {cm.relative_to(ROOT)} — injetado juce_generate_juce_header()")
            patched += 1
    if patched: log(f"{patched} CMakeLists.txt recebeu juce_generate_juce_header.")

# ----------------------------------------------------------- build/instala ---

def cmake_cache_stale(builddir: Path, src: Path) -> bool:
    cache = builddir / "CMakeCache.txt"
    if not cache.exists(): return False
    try: text = cache.read_text(encoding="utf-8", errors="ignore")
    except OSError: return True
    def norm(p): return str(p).replace("\\", "/").rstrip("/").lower()
    want_src, want_bin = norm(src.resolve()), norm(builddir.resolve())
    for line in text.splitlines():
        if line.startswith("CMAKE_HOME_DIRECTORY"):
            if norm(line.split("=", 1)[-1].strip()) != want_src: return True
        if line.startswith("CMAKE_CACHEFILE_DIR"):
            if norm(line.split("=", 1)[-1].strip().strip('"')) != want_bin: return True
    return False

def configure_build(src: Path, builddir: Path, targets):
    fix_juce_cmake(src)
    fix_juce_header(src)
    if cmake_cache_stale(builddir, src):
        log(f"{builddir.name}: cache de CMake de outra pasta detectado — limpando...")
        shutil.rmtree(builddir, ignore_errors=True)
    if platform.system() == "Windows":
        run(["cmake", "-S", src, "-B", builddir,
             "-G", "Visual Studio 17 2022", "-A", "x64", f"-DJUCE_SOURCE_DIR={JUCE}"])
        for t in targets:
            run(["cmake", "--build", builddir, "--config", "Release", "--target", t])
    else:
        run(["cmake", "-S", src, "-B", builddir, "-G", "Xcode", f"-DJUCE_SOURCE_DIR={JUCE}"])
        run(["cmake", "--build", builddir, "--config", "Release", "--target", *targets])

def find_artifacts(builddir: Path, prefix: str):
    """Retorna apenas BUNDLES (DIRETÓRIOS .vst3/.component).
    No Windows, o binário DENTRO do bundle também termina em .vst3 mas é ARQUIVO —
    tem que ser ignorado (era a causa do NotADirectoryError na cópia), assim como
    bundles aninhados. 'prefix' (se informado) filtra pelo nome do bundle."""
    out = []
    for p in builddir.rglob("*"):
        n = p.name.lower()
        if not (n.endswith(".vst3") or n.endswith(".component")):
            continue
        if not p.is_dir():
            continue  # binário interno do bundle (arquivo), não o bundle
        rel = p.relative_to(builddir).parts[:-1]
        if any(str(x).lower().endswith((".vst3", ".component")) for x in rel):
            continue  # bundle aninhado dentro de outro bundle
        if prefix and prefix.lower() not in n:
            continue
        out.append(p)
    return out

def copytree_retry(src: Path, dst: Path, attempts: int = 4, wait: float = 3.0):
    """Cópia com retry: travas transitórias (antivírus/indexador) dão WinError 32/267
    que somem segundos depois."""
    for i in range(attempts):
        try:
            shutil.copytree(src, dst, dirs_exist_ok=True)
            return
        except OSError as e:
            if i == attempts - 1: raise
            log(f"Cópia travada ({e}) — nova tentativa {i+2}/{attempts} em {wait}s...")
            time.sleep(wait)

def install_artifacts(artifacts):
    DIST.mkdir(exist_ok=True)
    if platform.system() == "Windows":
        user_vst3 = (Path(os.environ.get("LOCALAPPDATA", Path.home() / "AppData/Local"))
                     / "Programs" / "Common" / "VST3")
        user_vst3.mkdir(parents=True, exist_ok=True)
        for src in artifacts:
            if src.suffix.lower() == ".vst3":
                copytree_retry(src, user_vst3 / src.name)
                copytree_retry(src, DIST / src.name)
    else:
        vst3 = Path.home() / "Library/Audio/Plug-Ins/VST3"
        au = Path.home() / "Library/Audio/Plug-Ins/Components"
        vst3.mkdir(parents=True, exist_ok=True); au.mkdir(parents=True, exist_ok=True)
        for src in artifacts:
            dest = vst3 if src.suffix.lower() == ".vst3" else au
            copytree_retry(src, dest / src.name)
            copytree_retry(src, DIST / src.name)
    log("Artefatos instalados:")
    for a in artifacts: log(f"  {a.name}")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--clean", action="store_true")
    ap.add_argument("--no-install", action="store_true")
    a = ap.parse_args()

    system = platform.system()
    if system == "Windows": ensure_windows()
    elif system == "Darwin": ensure_macos()
    else: raise SystemExit("SO não suportado: apenas Windows e macOS.")

    ensure_juce()

    if a.clean:
        for d in (ROOT / "build-win", ROOT / "build-mac",
                  ROOT / "build-live-win", ROOT / "build-live-mac", DIST):
            if d.exists(): shutil.rmtree(d)

    artifacts = []
    for src, prefix in PROJECTS:
        tag = "win" if system == "Windows" else "mac"
        b = ROOT / f"build-{'live-' if prefix == 'BlackTieLive' else ''}{tag}"
        targets = [f"{prefix}_VST3"] + ([f"{prefix}_AU"] if system == "Darwin" else [])
        configure_build(src, b, targets)
        artifacts += find_artifacts(b, "")   # aceita qualquer bundle do próprio build

    if not artifacts:
        raise RuntimeError("CMake terminou mas nenhum VST3/AU foi encontrado.")

    if not a.no_install:
        install_artifacts(artifacts)

    log(f"PRONTO — artefatos em {DIST}")
    log("Reabra o Ableton Live ou faça Rescan dos plug-ins se ele estava aberto.")

# ------------------------------------------------------------ ponto de entrada

if __name__ == "__main__":
    code = 0
    try:
        main()
    except subprocess.CalledProcessError as e:
        code = e.returncode; print(f"\n[Black Tie] Comando falhou com código {e.returncode}")
    except SystemExit as e:
        code = e.code if isinstance(e.code, int) else 0
    except Exception as e:
        code = 1; print(f"\n[Black Tie] ERRO: {e}"); traceback.print_exc()
    if platform.system() == "Windows" and (code != 0 or IN_IDLE):
        input("\n[Black Tie] Pressione ENTER para fechar...")
    sys.exit(code)