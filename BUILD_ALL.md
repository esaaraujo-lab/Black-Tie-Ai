# Build both Black Tie products

The repository intentionally keeps the AI producer and performance plugin as two independent plug-in targets.

- `vst3/` — Black Tie AI (VST3 + AU on macOS)
- `performance/` — Black Tie Live (VST3 + AU on macOS)
- `scripts/install_all.py` — cross-platform bootstrap/build/install
- `installer/bootstrap-windows.ps1` — Windows bootstrap
- `installer/bootstrap-macos.command` — macOS bootstrap

JUCE 9.0.1 is pinned. If a local checkout exists at `third_party/JUCE`, it is reused. Otherwise the scripts clone the tag.
