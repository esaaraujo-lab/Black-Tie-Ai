# ChatGPT × Ableton AI Producer — FINAL 1.1

Native JUCE VST3/AU AI production agent with configurable OpenAI, Gemini, NVIDIA NIM and optional Ollama local/cloud providers.

## Core promise

The plugin does **not** require Ollama. Cloud APIs are first-class and are configured directly in the plugin UI. You can run one provider, or an ensemble of all enabled providers. Ollama is an optional provider family: local `http://127.0.0.1:11434/v1` and Ollama Cloud can be enabled independently.

The plugin contains the agent, provider routing, sample indexing, local audio transformation and third-party VST3/AU hosting. Ableton Live Set manipulation remains an explicit Live-side adapter because a VST3 cannot directly call Live's internal object model.

## Provider modes

- **1 API**: the provider selected in the main UI is used.
- **Todas as APIs**: every enabled/configured provider is used sequentially; the first successful plan becomes the candidate and subsequent providers validate/repair it.
- **Ollama**: if local is enabled it is preferred; cloud is the fallback when local is disabled/unavailable.
- OpenAI, Gemini and NVIDIA do not depend on Ollama.

## UI configuration

Open **API / PROVIDERS** inside the VST and set, for each provider:

- enabled/disabled
- API base URL
- model name
- API key

The base URL is expected to expose an OpenAI-compatible `/chat/completions` endpoint. This makes the addresses editable rather than hard-coded.

API keys are not saved into project state unless **Save API keys in the Ableton project state** is enabled. This is intentionally opt-in because DAW project files are not a secure secret vault.

## Build

Windows: `installer/bootstrap-windows.ps1`

macOS: `installer/bootstrap-macos.command`

The build fetches the pinned JUCE tag when a local checkout is not supplied. Native VST3/AU compilation and Ableton integration tests must be run on the target OS/DAW.

## Architecture

```text
VST3/AU
  ├─ ProducerAgent
  ├─ ProviderRouter
  │   ├─ OpenAI
  │   ├─ Gemini
  │   ├─ NVIDIA NIM
  │   └─ Ollama Local / Cloud
  ├─ SampleIndex
  ├─ LocalAudioEngine
  └─ PluginHost (VST3, AU on macOS)
             │
             └── Live-side adapter (optional but required for direct Set edits)
```

## What is deliberately not claimed

The plugin cannot directly manipulate Ableton's Live Object Model from inside a VST3. For Set/Arrangement changes, a Live-side integration is required. Ableton's current Extensions SDK is the preferred path when the user runs a compatible Live 12.4.5+ Suite beta; Max for Live/other adapters remain possible alternatives.

## Verification

Run:

```bash
./tests/smoke.sh
```

The source audit checks provider routing, state persistence, JUCE hosting/scanning calls, audio-thread safety and required attribution files. Native build certification requires Windows/macOS toolchains and a real Ableton Live installation.

## Credits

See `THIRD_PARTY_NOTICES.md` and `docs/19_ATTRIBUTION_AND_LICENSES.md` for copied-code policy, licenses and inspiration credits.
