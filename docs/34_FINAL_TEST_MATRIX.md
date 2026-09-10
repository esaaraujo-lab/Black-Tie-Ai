# Final test matrix

## Static tests executed in this environment

- Source audit: PASS
- Manifest parse: PASS
- Provider router presence: PASS
- Selected-provider routing: PASS by source assertion
- All-provider routing: PASS by source assertion
- API configuration state fields: PASS by source assertion
- Optional Ollama architecture: PASS by source assertion
- JUCE `unique_ptr<AudioPluginInstance>` ownership: PASS by source inspection
- Non-blocking audio callback lock: PASS by source inspection
- DSP preparation: PASS by source inspection
- Large-file integer guards: PASS by source inspection
- Attribution documents: PASS

## Tests that require the target machine

- Windows MSVC Release build
- macOS Xcode Release build
- VST3 validator/pluginval
- Ableton Live VST3 scan
- Real VST3 third-party hosting
- Real AU hosting on macOS
- Live Extensions SDK build against the exact beta SDK installed by the user
- Live Arrangement end-to-end test
- API calls with real credentials
- Ollama local model test
- Ollama Cloud test
- CPU/latency measurements in real-time playback
- Windows/macOS code signing and notarization

A source package is not labeled binary-certified until these tests pass on the target platforms.
