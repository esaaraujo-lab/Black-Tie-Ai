# FINAL 1.1 changelog

## Provider architecture

- Added `ProviderRouter`.
- Added explicit single-provider and all-provider modes.
- Fixed selected-provider routing in single mode.
- Added independent Ollama Local and Ollama Cloud slots.
- Removed any architectural dependency on Ollama for OpenAI/Gemini/NVIDIA.
- Made endpoint, model and API key editable inside the VST UI.
- Added opt-in project-state persistence for API keys with a visible warning.

## JUCE host

- Confirmed `AudioPluginFormatManager::createPluginInstance()` ownership semantics against current JUCE documentation.
- Corrected parameter-ID lookup to use `AudioProcessorParameter::getParameterID()`.
- Changed the real-time processing lock to `ScopedTryLock` to avoid blocking the audio callback behind scanning/loading operations.
- Kept plugin scanning isolated from the normal agent/network path.

## Audio engine

- Added large-file bounds checks.
- Added safe BPM transform bounds.
- Added explicit DSP preparation.
- Preserved the existing local transformations: reverse, stutter, gain, filtering, saturation and fades.

## Documentation

- Reworked attribution to distinguish copied code from inspiration.
- Added current official API verification for OpenAI, Gemini, NVIDIA NIM and Ollama.
- Added final issue ledger and test matrix.
- Documented the unavoidable Ableton boundary: a VST cannot directly manipulate Live's internal object model; a Live-side integration is required for Set edits.
- Preserved the future live-performance boundary for keyboard/MIDI/Kaoss Pad synchronization without pretending it is already implemented.
