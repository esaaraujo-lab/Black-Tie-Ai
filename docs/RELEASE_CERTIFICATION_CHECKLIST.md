# Release Certification Checklist

## Source-level

- [x] Provider independence: OpenAI/Gemini/NVIDIA/Ollama are separate entries.
- [x] Ollama optional.
- [x] Local Ollama can be offline; cloud endpoint is separate.
- [x] No network operation in `processBlock`.
- [x] No JSON parsing in `processBlock`.
- [x] No plugin scan/load in `processBlock`.
- [x] Black Tie Live is a separate target.
- [x] Live transport is queried only from `processBlock` through JUCE AudioPlayHead.
- [x] Third-party binaries are not redistributed.
- [x] Upstream inspiration and licenses documented.

## Native release

- [ ] Windows Release build on supported Windows/MSVC.
- [ ] macOS Release build on supported Xcode/architectures.
- [ ] VST3 validator/pluginval.
- [ ] AU validation on macOS.
- [ ] Ableton Live integration test.
- [ ] Real provider API smoke tests.
- [ ] Ollama offline and cloud tests.
- [ ] Third-party VST3 compatibility matrix.
- [ ] Code signing/notarization.
- [ ] Hardware MIDI/clock tests for Black Tie Live.
