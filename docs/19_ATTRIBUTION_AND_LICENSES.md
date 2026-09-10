# Attribution, provenance and licenses

This document separates **code copied**, **code used as a dependency**, and **architectural inspiration**. No community repository is represented as original work when it was only studied or used as inspiration.

## JUCE — dependency

JUCE is the native C++ framework used by the plugin. The project pins JUCE 9.0.1 at build time. JUCE licensing is dual-track; the distributor must choose the applicable JUCE license/commercial arrangement for the intended distribution. The JUCE source is not silently relicensed by this project.

Official: https://github.com/juce-framework/JUCE
API: https://docs.juce.com/

## Ableton — API/SDK dependency

Ableton Live APIs and the Ableton Extensions SDK are Ableton technologies. This project uses documented interfaces and does not redistribute proprietary SDK material. The Extensions SDK is currently public beta for Live 12.4.5+ Suite beta.

Official: https://www.ableton.com/en/live/extensions/
SDK docs: https://ableton.github.io/extensions-sdk/

## Community projects — inspiration only unless a future vendored notice says otherwise

- `ahujasid/ableton-mcp` — natural-language Ableton control, Arrangement composition, MIDI and device workflows. **Inspiration; no code copied.** MIT.
- `wstierhout/ableton-live-mcp` — typed Live tooling, device/parameter introspection, mixing/mastering and safety-oriented workflows. **Inspiration; no code copied.** Verify the upstream license/version before redistribution.
- `idx3d/ableton-mcp-extension` — Ableton Extensions boundary, local MCP transport and safety/token concepts. **Inspiration; no code copied.**
- `nicholasbien/ableton-mcp-pro` — Arrangement read/modify/write and audio/MIDI placement concepts. **Inspiration; no code copied.**
- `gary4juce` — AI audio generation in a JUCE plugin and local/remote generation backends. **Inspiration; no code copied.** Upstream is AGPL-3.0-only; its model/backend licenses are separate.
- `LLMidi` — natural-language MIDI generation in a JUCE VST3 with local/online LLM modes. **Inspiration; no code copied.**
- `Loophole` — compact packaged Ableton Extension concept. **Inspiration; no code copied.**
- `PluginDB` — machine-readable global plugin catalogue concept. **Inspiration; no code copied.**
- `Tracktion/pluginval` — plugin validation tool recommended for release QA. **Tool reference; binary not redistributed.**

## Model/backend projects

The project may optionally integrate external model backends such as Stable Audio, ACE-Step, MusicGen, llama.cpp or Ollama. Model weights and backend software are separate works with their own licenses. They are not embedded in this source package.

## Copy policy

No source file in this repository is intentionally copied from the community projects above. If a future release vendors code, the vendor directory must retain the upstream copyright/license headers and this document must be updated with the exact commit/tag and license.

## Commercial/plugin assets

Commercial VST3/AU plugins, Ableton Packs, user samples, API credentials and model weights are not redistributed. The end user supplies them under their own licenses.
