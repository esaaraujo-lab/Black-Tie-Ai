#!/usr/bin/env python3
"""Black Tie AI Producer unattended-ish bootstrap/build/install helper.
Installs missing build prerequisites using the native package manager where possible,
clones JUCE, configures CMake, builds both plugins, and copies artifacts to standard folders.
"""
from __future__ import annotations
import os, platform, shutil, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JUCE = ROOT / "third_party" / "JUCE"


def run(cmd, cwd=ROOT, env=None):
    print("+", " ".join(map(str, cmd)))
    subprocess.run([str(x) for x in cmd], cwd=cwd, env=env, check=True)


def which(name):
    return shutil.which(name)


def ensure_windows_tools():
    if not which("winget"):
        raise RuntimeError("winget is required for unattended Windows bootstrap; install App Installer from Microsoft Store.")
    installs = [
        ("Git.Git", "Git.Git"),
        ("Kitware.CMake", "Kitware.CMake"),
        ("Python.Python.3.13", "Python.Python.3.13"),
    ]
    for _, ident in installs:
        if (ident == "Git.Git" and not which("git")) or (ident == "Kitware.CMake" and not which("cmake")) or (ident == "Python.Python.3.13" and not which("python")) :
            run(["winget", "install", "--id", ident, "-e", "--silent", "--accept-source-agreements", "--accept-package-agreements"])
    if not which("cl"):
        run(["winget", "install", "--id", "Microsoft.VisualStudio.2022.BuildTools", "-e", "--silent", "--accept-source-agreements", "--accept-package-agreements", "--override", "--wait --passive --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended"])


def ensure_macos_tools():
    if not which("xcode-select"):
        raise RuntimeError("xcode-select is unavailable; macOS developer tools are required.")
    if subprocess.run(["xcode-select", "-p"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode != 0:
        # Apple intentionally gates Command Line Tools installation behind system UI/consent.
        raise RuntimeError("Apple Command Line Tools are not installed. macOS requires the user/system administrator to approve this installation.")
    if not which("brew"):
        run(["/bin/bash", "-c", "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"], cwd=ROOT)
    for tool, formula in (("git", "git"), ("cmake", "cmake")):
        if not which(tool): run(["brew", "install", formula])


def ensure_juce():
    JUCE.parent.mkdir(parents=True, exist_ok=True)
    if not (JUCE / "CMakeLists.txt").exists():
        run(["git", "clone", "--depth", "1", "--branch", "9.0.1", "https://github.com/juce-framework/JUCE.git", str(JUCE)])


def build():
    system = platform.system()
    if system == "Windows":
        builddir = ROOT / "build-win"
        run(["cmake", "-S", str(ROOT / "vst3"), "-B", str(builddir), "-G", "Visual Studio 17 2022", "-A", "x64", f"-DJUCE_SOURCE_DIR={JUCE}"])
        run(["cmake", "--build", str(builddir), "--config", "Release", "--target", "ChatGPTAbletonAIProducer_VST3"])
        run(["cmake", "-S", str(ROOT / "performance"), "-B", str(ROOT / "build-live-win"), "-G", "Visual Studio 17 2022", "-A", "x64", f"-DJUCE_SOURCE_DIR={JUCE}"])
        run(["cmake", "--build", str(ROOT / "build-live-win"), "--config", "Release", "--target", "BlackTieLive_VST3"])
        return builddir, ROOT / "build-live-win"
    if system == "Darwin":
        b1 = ROOT / "build-mac"
        b2 = ROOT / "build-live-mac"
        run(["cmake", "-S", str(ROOT / "vst3"), "-B", str(b1), "-G", "Xcode", f"-DJUCE_SOURCE_DIR={JUCE}"])
        run(["cmake", "--build", str(b1), "--config", "Release", "--target", "ChatGPTAbletonAIProducer_VST3", "ChatGPTAbletonAIProducer_AU"])
        run(["cmake", "-S", str(ROOT / "performance"), "-B", str(b2), "-G", "Xcode", f"-DJUCE_SOURCE_DIR={JUCE}"])
        run(["cmake", "--build", str(b2), "--config", "Release", "--target", "BlackTieLive_VST3", "BlackTieLive_AU"])
        return b1, b2
    raise RuntimeError("Only Windows and macOS are supported by this installer.")


def install(b1, b2):
    dist = ROOT / "dist"
    dist.mkdir(exist_ok=True)
    candidates = list((b1 / "ChatGPTAbletonAIProducer_artefacts" / "Release" / "VST3").glob("*.vst3")) + list((b2 / "BlackTieLive_artefacts" / "Release" / "VST3").glob("*.vst3"))
    if platform.system() == "Darwin":
        candidates += list((b1 / "ChatGPTAbletonAIProducer_artefacts" / "Release" / "AU").glob("*.component"))
        candidates += list((b2 / "BlackTieLive_artefacts" / "Release" / "AU").glob("*.component"))
    if not candidates:
        raise RuntimeError("Build completed but no plugin artifacts were found.")

    for src in candidates:
        target = dist / src.name
        if src.is_dir():
            shutil.copytree(src, target, dirs_exist_ok=True)
        else:
            shutil.copy2(src, target)

    # Install into per-user standard plugin locations first, avoiding elevation.
    if platform.system() == "Windows":
        install_dir = Path(os.environ.get("LOCALAPPDATA", str(Path.home() / "AppData/Local"))) / "Programs" / "Common Files" / "VST3"
        install_dir.mkdir(parents=True, exist_ok=True)
        for src in candidates:
            if src.suffix.lower() == ".vst3":
                shutil.copytree(src, install_dir / src.name, dirs_exist_ok=True)
    else:
        vst_dir = Path.home() / "Library/Audio/Plug-Ins/VST3"
        au_dir = Path.home() / "Library/Audio/Plug-Ins/Components"
        vst_dir.mkdir(parents=True, exist_ok=True); au_dir.mkdir(parents=True, exist_ok=True)
        for src in candidates:
            if src.suffix.lower() == ".vst3": shutil.copytree(src, vst_dir / src.name, dirs_exist_ok=True)
            elif src.suffix.lower() == ".component": shutil.copytree(src, au_dir / src.name, dirs_exist_ok=True)

    print("Artifacts installed:")
    for x in candidates:
        print(" ", x)


def main():
    system = platform.system()
    if system == "Windows": ensure_windows_tools()
    elif system == "Darwin": ensure_macos_tools()
    else: raise SystemExit("Unsupported OS")
    ensure_juce()
    b1, b2 = build()
    install(b1, b2)
    print("Black Tie AI + Black Tie Live build/install completed.")

if __name__ == "__main__": main()
