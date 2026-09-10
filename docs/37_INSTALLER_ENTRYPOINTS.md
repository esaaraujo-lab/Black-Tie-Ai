# Installer entry points

There is now one canonical installer implementation:

`install.py`

Wrappers:

- Windows: `installer/bootstrap-windows.ps1`
- macOS: `installer/bootstrap-macos.command`
- legacy-compatible: `scripts/install_all.py`

All wrappers call the same Python implementation. This prevents the Windows and Python installers from drifting apart.

## Commands

`python install.py` — bootstrap dependencies, clone JUCE, configure, compile and install.

`python install.py --clean` — remove generated build/dist directories before rebuilding.

`python install.py --no-install` — build but leave artifacts in `dist/`.
