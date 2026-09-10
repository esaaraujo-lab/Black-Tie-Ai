# Community / Upstream Credits

This project contains original code written for Black Tie AI and Black Tie Live. The following projects and specifications were reviewed as technical references or inspiration. They are **not** represented as copied code unless a file explicitly says so.

## JUCE

JUCE is the framework used to build the VST3/AU products and to host third-party plug-ins. Reference: https://github.com/juce-framework/JUCE and the JUCE CMake API.

## Steinberg VST3

The VST3 Developer Portal and SDK documentation were used for module/plug-in architecture and validation concepts. VST3 SDK is MIT licensed according to Steinberg's portal.

## Apple Audio Unit

Apple Developer Documentation was used for AU/AUv3 distinctions and host/plugin lifecycle constraints.

## Ableton Extensions

Ableton's Extensions SDK documentation was used to define the boundary for Set-level automation. Extensions run with Live and can interact with tracks, clips, MIDI, devices and tempo. The SDK itself is obtained from Ableton's beta program and is not redistributed here.

## Community projects reviewed

- ahujasid/ableton-mcp — Ableton control/composition concepts.
- wstierhout/ableton-live-mcp — tool surface, analysis and robust command handling concepts.
- idx3d/ableton-mcp-extension — current Ableton Extensions architecture and capability map.
- nicholasbien/ableton-mcp-pro — arrangement editing and direct Live workflow concepts.
- betweentwomidnights/gary4juce — AI audio generation inside a JUCE plug-in concept.
- DirtyBeastAfterTheToad/LLMidi — LLM-to-MIDI VST3 concept.
- OthmanAdi/loophole — single `.ablx` Ableton Extension deployment concept.
- blakebratcher/plugindb — plug-in catalog/metadata concept.
- Tracktion/pluginval — plug-in validation concept.

Always consult each upstream repository's LICENSE before redistributing any derivative code. This repository intentionally does not ship proprietary third-party plug-in binaries or the Ableton Extensions SDK.
