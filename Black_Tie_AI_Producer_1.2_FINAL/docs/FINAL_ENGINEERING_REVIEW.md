# Black Tie AI / Black Tie Live — Final Engineering Review 1.2

Date: 2026-09-10

## Scope

This review treats the project as a production audio plug-in system. It cross-checks the source against current public JUCE CMake/API documentation, Steinberg VST3 documentation, Apple Audio Unit documentation, Ableton Extensions documentation, and selected community implementations.

## Architecture decisions

1. **Cloud AI is direct.** OpenAI, Gemini and NVIDIA NIM are independent HTTP providers. Ollama is optional and has both local and cloud profiles.
2. **The VST owns the agent.** No Node/Python runtime is required for AI requests during normal plugin use.
3. **Ableton Set manipulation is a separate host integration boundary.** A VST cannot acquire arbitrary Live Set/Object Model privileges merely by being inserted on a track. Ableton Extensions are the supported current route for Set-level automation; Max for Live remains relevant for deep device/creative work.
4. **Third-party plugin hosting is isolated in the JUCE host layer.** The AI Producer can host plugins that are installed/licensed on the machine; it does not redistribute proprietary plugins.
5. **Audio-thread safety is a release gate.** Network, scanning, file decoding, JSON parsing, plugin instantiation and UI operations must not execute from `processBlock`.
6. **Black Tie Live is a separate MIDI-performance product.** It is intentionally independent from Black Tie AI and is the foundation for future controller/Kaoss/MIDI-clock work.

## Current verified model defaults

- OpenAI: `gpt-5.6-luna` (Chat Completions supported).
- Gemini: `gemini-3.8-flash` (verify availability at build/use time because model catalog changes).
- NVIDIA: `meta/llama-3.3-70b-instruct` as a configurable example; NIM deployments should query `/v1/models` and use the served ID.
- Ollama Local: `qwen3:8b` as a configurable example.

## Provider behavior

### Single

Exactly one selected provider is called. Ollama is never silently substituted for a cloud provider.

### All

Enabled providers are called sequentially. Each later provider receives the prior JSON candidate and is asked to validate/repair it. This is a deterministic review chain, not a statistical consensus claim.

### Ollama

Ollama Local is optional. If installed, the user can use local models offline and, where supported/configured, cloud models through Ollama's OpenAI-compatible endpoint. The plugin does not require Ollama for OpenAI/Gemini/NVIDIA.

## VST3/AU review

JUCE's `juce_add_plugin` is used for VST3 and AU. VST3 is cross-platform; AU is macOS-only. Plugin manufacturer/plugin codes are fixed and valid for AU requirements. The VST3/AU targets use C++17.

Steinberg's VST3 documentation describes the module factory and `GetPluginFactory` boundary; JUCE supplies the wrapper/host integration used here. Apple documents AUv3 as a distinct App Extension model; this project builds the classic macOS AU format requested for DAW hosting, not an AUv3 app extension.

## Known hard boundaries

- A VST3 inserted on an Ableton track cannot, by itself, rewrite arbitrary tracks/clips in the Live Set.
- Ableton Extensions currently require an Extensions-capable Live release and the Ableton SDK. The project therefore treats the Extension as an optional host-side component rather than pretending it is embedded in the VST binary.
- Some third-party plugins are not safe to instantiate in arbitrary hosts; production deployment should use crash isolation/plugin validation before enabling automatic hosting.
- Code signing/notarization requires platform certificates and cannot be fabricated by a build script.

## Phase 2: Black Tie Live

Black Tie Live is a MIDI effect with:

- MIDI input/output;
- note pass-through;
- channel filtering;
- semitone transpose;
- MIDI clock output at 24 pulses per quarter note;
- saved state;
- dedicated UI.

Future performance work is intentionally layered above this core: external keyboard mapping, CC/MIDI-learn, scene/clip control through Ableton's host integration, controller feedback, Kaoss Pad mappings, quantized macro changes, and safe live-set recovery.

## Test gates

Static tests can validate source invariants, CMake target structure, JSON, scripts and documentation. Real release certification additionally requires:

- Windows native build;
- macOS native build;
- VST3 validation with Steinberg validator/pluginval;
- AU validation on macOS;
- loading in the exact target Ableton Live version;
- cloud provider calls with real credentials;
- Ollama offline/online tests;
- third-party plugin compatibility matrix;
- MIDI hardware tests.
