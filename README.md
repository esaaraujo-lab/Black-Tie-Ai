# Black Tie AI Producer + Black Tie Live — FINAL 1.2.1

**Canonical installer:** `install.py`

Windows: `installer\bootstrap-windows.ps1`
macOS: `installer/bootstrap-macos.command`

The installers now share exactly the same Python implementation.

# Black Tie AI + Black Tie Live — 1.2

Two independent JUCE products for electronic-music production and live performance.

## Black Tie AI

An AI producer embedded in a VST3/AU plug-in. OpenAI, Gemini, NVIDIA NIM and Ollama are independent providers configured inside the plug-in. Ollama is optional.

The producer can reason over local samples, create/transform audio, generate MIDI plans, host installed third-party plug-ins through JUCE, and prepare Ableton actions.

## Black Tie Live

A separate low-latency MIDI performance plug-in. Current core: MIDI pass-through, channel filtering, transpose and MIDI clock output. It is the foundation for the next controller/Kaoss Pad/live-set phase.

## Important host boundary

A VST cannot directly rewrite arbitrary Ableton Live Set objects. Set-level automation uses the Ableton Extensions SDK adapter when available. This is documented rather than hidden.

## Build

Run:

- Windows: `installer/bootstrap-windows.ps1`
- macOS: `installer/bootstrap-macos.command`
- Cross-platform Python: `python3 scripts/install_all.py`

The scripts clone JUCE 9.0.1 and build both products. See `docs/INSTALLATION.md`.
