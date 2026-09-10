# Final issue ledger — 2026-09-10

| ID | Issue | Resolution | Status |
|---|---|---|---|
| F-01 | Cloud providers depended on Ollama | Direct provider HTTP clients are first-class | Fixed |
| F-02 | Provider selection was ignored in single mode | Router receives selected provider only in single mode | Fixed |
| F-03 | No in-plugin API configuration UI | Endpoint/model/key fields added for OpenAI/Gemini/NVIDIA/Ollama Local/Ollama Cloud | Fixed |
| F-04 | No all-provider mode | Deterministic ensemble/review router | Fixed |
| F-05 | Ollama cloud and local were conflated | Separate provider slots | Fixed |
| F-06 | API keys could accidentally be serialized | Opt-in `rememberKeys` plus warning | Fixed |
| F-07 | PluginHost could block audio thread on mutex | `ScopedTryLock`; block passes through if mutation owns lock | Fixed |
| F-08 | PluginHost ownership semantics were easy to misread | Uses JUCE `unique_ptr` return and transfers ownership once | Fixed |
| F-09 | Audio transformer could overflow `int` | Size checks and 8x transform cap | Fixed |
| F-10 | DSP filters were not explicitly prepared | `prepare()` added | Fixed |
| F-11 | Extension source claimed a complete SDK implementation without SDK | Kept as explicit adapter boundary | Fixed/limited |
| F-12 | Native Ableton control was implied to be possible from a VST alone | Documentation now states Live-side integration is required for Set edits | Fixed |
| F-13 | Binary certification was overstated | Native build/Live/pluginval gates explicitly marked pending | Fixed |

## Non-bugs / deliberate constraints

- Ollama is optional.
- A VST3 cannot directly access Ableton's internal Live Object Model.
- Third-party commercial plugin licenses are the user's responsibility.
- API keys are not a secure secret store; project-state persistence is opt-in.
- The current local audio transform is resampling-style BPM conversion, not pitch-preserving time-stretch.
