# ChatGPT × Ableton AI Producer — FINAL 1.1

This release is the final source baseline for the current scope.

### User-requested requirements implemented

- Direct OpenAI API from inside the VST.
- Direct Gemini API from inside the VST.
- Direct NVIDIA NIM API from inside the VST.
- Ollama is optional, not a prerequisite.
- Ollama local and Ollama Cloud can be configured independently.
- One-provider mode.
- All-provider ensemble mode.
- Provider endpoint/model/key configuration inside the VST UI.
- Optional API-key persistence with warning.
- JUCE third-party VST3/AU hosting foundation.
- Local sample indexing and audio transformation.
- Agent planning for complete electronic arrangements.
- Live-side adapter boundary for Set/Arrangement mutation.
- Extensive source audit, attribution and release-gate documentation.

### Important compatibility boundary

A VST3 cannot directly call Ableton Live's internal Live Object Model. The Live-side Extensions SDK is the preferred current route for direct Set manipulation in compatible Live 12.4.5+ Suite beta builds. The plugin itself remains independent of Ollama and Node/Python for its AI/provider/audio core.

### Binary status

This source package is reproducible, but Windows/macOS native binaries were not certified in the current sandbox. A real target build, plugin validation, signing and Ableton integration test are still required before commercial distribution.
