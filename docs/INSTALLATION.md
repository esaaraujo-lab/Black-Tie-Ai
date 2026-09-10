# Installation

## Windows

Run `installer/bootstrap-windows.ps1` from PowerShell. It checks/installs Git, CMake and Visual Studio Build Tools where `winget` permits unattended installation, clones JUCE 9.0.1, builds both VST3 products and places the resulting artifacts in `dist/`.

If Windows blocks execution policy, run PowerShell with the normal script execution policy appropriate to your machine; the script does not disable security globally.

## macOS

Run `installer/bootstrap-macos.command`. It checks Xcode Command Line Tools, installs Homebrew packages if missing, clones JUCE 9.0.1, builds VST3 and AU targets for both products, and writes them to `dist/`.

Apple deliberately requires approval for Command Line Tools installation on systems where it is absent; no third-party script can legitimately bypass that consent requirement.

## Python

`python3 scripts/install_all.py` is the cross-platform bootstrap. It performs the same build orchestration and is useful for CI or technical users.

## Install products

Copy the generated `.vst3` bundles to the normal VST3 location, or use Ableton's plug-in discovery. On macOS, AU `.component` bundles can be placed in `~/Library/Audio/Plug-Ins/Components` if AU hosting is desired.

## Important: Ableton Set automation

The AI agent is embedded in the VST. However, Set-level editing requires an Ableton-side integration. The repository keeps an Extensions adapter for that purpose. A pure VST cannot bypass the host's plug-in sandbox/contract and directly edit arbitrary Live tracks.

## Automatic per-user installation

After building, the Python/bootstrap scripts also copy VST3 bundles to a per-user standard VST3 directory so administrator privileges are normally unnecessary:

- Windows: `%LOCALAPPDATA%/Programs/Common Files/VST3`
- macOS: `~/Library/Audio/Plug-Ins/VST3`
- macOS AU: `~/Library/Audio/Plug-Ins/Components`

The scripts do not silently elevate privileges. If a machine policy prevents the package manager/compiler installation, that is reported explicitly.
