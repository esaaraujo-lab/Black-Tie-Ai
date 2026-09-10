# Integration matrix

| Upstream | What we take | What stays external |
|---|---|---|
| JUCE | framework/build/hosting APIs | JUCE checkout is downloaded at build time |
| LLMidi | in-plugin LLM→MIDI UX pattern | upstream source/model binaries |
| gary4juce | AI-audio backend boundary | model/runtime/backends |
| Loophole | one-file Live Extension install strategy | Ableton SDK |
| ableton-mcp-extension | Live MCP/typed command architecture | Ableton SDK |
| PluginDB | global plugin catalog concept | catalog licensing/updates |
| pluginval | QA gate | validator binary downloaded for CI |
