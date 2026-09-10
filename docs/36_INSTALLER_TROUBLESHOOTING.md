# Installer troubleshooting — final 1.2.1

## Windows

Run PowerShell from the extracted project directory:

`powershell -ExecutionPolicy Bypass -File .\installer\bootstrap-windows.ps1`

Or simply:

`py .\install.py`

The PowerShell script is now a visible wrapper around the same Python installer, so both entry points execute the same code path. The installer prints every command and stops on errors instead of silently returning.

### If PowerShell says nothing happened

Do not double-click the `.ps1`. Run it from PowerShell with the explicit command above. If script execution policy blocks it, use the `-ExecutionPolicy Bypass` invocation above. The installer itself prints a banner immediately.

### MSVC

The installer installs Visual Studio 2022 Build Tools through winget when the MSVC compiler is missing. It then uses `VsDevCmd.bat` when necessary so CMake can find MSVC even when `cl.exe` is not in the original PATH.

### winget

Windows' package manager is Microsoft App Installer. If `winget` is genuinely unavailable, that component must be installed by Windows; there is no reliable way for a third-party script to bootstrap the Microsoft Store/App Installer itself on every Windows edition.

## macOS

Run:

`chmod +x installer/bootstrap-macos.command`

then double-click it or execute `./installer/bootstrap-macos.command`.

Apple's Command Line Tools installation is intentionally gated by macOS and can require an approval dialog. This is the only first-run prerequisite the script cannot legally/technically guarantee to install without system consent.

After the tools exist, the same Python installer clones JUCE, configures CMake, builds VST3/AU, and installs them to the per-user plugin folders.

## After installation

The normal installed products are:

- `Black Tie AI Producer.vst3`
- `Black Tie Live.vst3`
- macOS additionally receives AU `.component` builds.

If Ableton Live was open while installing, restart it and rescan plug-ins.
