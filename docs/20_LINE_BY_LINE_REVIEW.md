# FINAL 1.1 — line-by-line/minutia review

Date: 2026-09-10

This is a source audit, not a claim of native binary certification. Every project source file was inspected after the final provider-configuration changes. The review cross-checks the implementation against current JUCE API documentation, current Ableton Extensions documentation, and the selected community projects.

## 1. ProviderClient.cpp

- HTTP is executed by the editor worker thread, never `processBlock`.
- The base URL is configurable; `/chat/completions` is appended consistently.
- Authorization is sent only when a key is present.
- Local Ollama does not require an API key.
- Provider-specific structured-output fields are not forced on every OpenAI-compatible endpoint.
- Responses are parsed before agent use.
- JSON fenced blocks are stripped before validation.
- Invalid JSON is rejected instead of being executed.

## 2. ProviderRouter.cpp

- Single mode accepts exactly the selected provider list.
- All mode calls every enabled provider and preserves the last valid candidate.
- A failed provider does not prevent the remaining providers from being tried.
- The router does not require Ollama.
- Ollama local/cloud are represented as independent endpoints.
- The ensemble protocol is deterministic and explicitly documented; it is not presented as a hidden consensus algorithm.

## 3. PluginProcessor.cpp/.h

- Provider endpoints/models/keys are first-class plugin state.
- OpenAI, Gemini, NVIDIA NIM and Ollama are configured inside the plugin.
- `rememberKeys=false` omits secrets from serialized plugin state.
- `rememberKeys=true` is explicitly opt-in and warned about in the UI.
- The selected provider is honored in single-provider mode.
- All-provider mode ignores the selected provider intentionally.
- Live URL is normalized and has a localhost default.
- Audio processing contains no network or JSON generation.
- The plugin exposes VST3 and AU wrappers through JUCE CMake configuration.

## 4. PluginEditor.cpp/.h

- Network requests execute on a dedicated JUCE `Thread`.
- UI changes are marshalled back with `MessageManager::callAsync`.
- Provider settings are editable directly in the plugin.
- API keys are masked in the UI.
- Settings are committed before an agent request.
- The UI has a clear single/all provider mode.
- Execute refuses malformed plans.
- Stop remains available while work is running.

## 5. PluginHost.cpp/.h

- JUCE's `AudioPluginFormatManager` is used for VST3 and AU discovery/creation.
- `createPluginInstance()` returns a `std::unique_ptr<AudioPluginInstance>` and ownership is transferred exactly once to the `OwnedArray`.
- Scanning uses `PluginDirectoryScanner` and records the failed/dead-man entries.
- The audio callback uses `ScopedTryLock` rather than blocking on a potentially long plugin load/scan operation. If a mutation currently owns the lock, that block is passed through unchanged instead of stalling the audio thread.
- This is a safe real-time fallback, although production certification should still enforce plugin graph mutation at safe transport/lifecycle boundaries.
- Plugin parameter writes are normalized because JUCE host parameters are represented as normalized values; display conversion belongs to the parameter implementation.

## 6. LocalAudioEngine.cpp

- Input existence and decoder support are checked.
- Very large files are rejected before integer truncation.
- Decoder failure is reported.
- BPM transforms are capped to 8x to prevent accidental enormous allocations.
- The operation is explicitly a resampling-style BPM transform and does not claim pitch-preserving time stretch.
- Reverse, stutter, gain, filters, saturation and fades are local/offline operations.
- JUCE DSP filters are prepared before processing.
- Output is written as WAV with a checked output stream.

## 7. SampleIndex.cpp

- Only supported audio extensions are indexed.
- Recursive directory traversal is used.
- Invalid roots are skipped.
- IDs are derived from file metadata rather than being used as arbitrary model-generated paths.
- The Live-side adapter must still validate any path before importing it into a Set.

## 8. Ableton extension

The current extension directory is an adapter boundary. It must be compiled against the exact Ableton Extensions SDK package supplied by Ableton's beta program. It intentionally does not pretend that a generic TypeScript object is the SDK implementation.

Ableton's official documentation currently says Extensions are available in Live 12.4.5+ public beta and can read/edit tracks, clips, MIDI, devices, tempo and Set structure. They run on Node.js and are separate from Max for Live.

## 9. CMake

- JUCE is pinned to 9.0.1.
- Existing local JUCE checkout is preferred for reproducible/offline builds.
- FetchContent is the fallback when network access is available.
- VST3 and AU are built.
- Third-party VST3/AU hosting is enabled explicitly.

## 10. Known release gates

The following cannot be truthfully marked PASS in this environment:

1. Visual Studio/Xcode native compilation against the actual JUCE checkout.
2. pluginval execution against the produced binary.
3. Ableton Live load/scan test.
4. Exact Ableton Extensions SDK package compilation.
5. Third-party plugin crash/compatibility matrix.
6. Code signing/notarization.
7. End-to-end cloud API calls using real credentials.

These are release gates, not hidden assumptions.

## Final 1.2 additions

- Black Tie Live is now a separate JUCE MIDI-effect target.
- It uses `IS_MIDI_EFFECT`, MIDI input/output and `kAudioUnitType_MIDIProcessor`, matching JUCE CMake options.
- Transport/BPM reads use `AudioPlayHead::getPosition()` only from `processBlock()`, matching JUCE documentation.
- MIDI clock uses the standard 24 pulses per quarter note representation.
- Eight CC macro slots are atomic and do not allocate or perform network I/O in the audio callback.
- The installer builds both products from the same pinned JUCE checkout.
