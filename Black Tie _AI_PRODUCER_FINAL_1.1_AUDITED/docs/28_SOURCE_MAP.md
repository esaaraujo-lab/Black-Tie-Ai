# Source map

- `vst3/`: JUCE plugin target and UI.
- `agent_core/ProviderClient.*`: provider-independent LLM HTTP client.
- `agent_core/ProducerAgent.*`: strict song/master plan generation.
- `agent_core/SampleIndex.*`: local sample inventory.
- `agent_core/LocalAudioEngine.*`: local deterministic audio transformations.
- `agent_core/PluginHost.*`: optional in-plugin third-party VST3/AU hosting.
- `ableton_extension/`: Live-side boundary; the exact SDK binding remains isolated here.
- `docs/`: engineering review, compatibility, attribution, build and certification material.
- `.github/workflows/`: reproducible target-native build pipeline.
