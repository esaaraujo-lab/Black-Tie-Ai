# Final test plan

## Static checks

- CMake configuration on Windows and macOS
- C++ compilation with pinned JUCE
- TypeScript compilation against the installed Ableton Extensions SDK
- Python/Max compatibility fallbacks
- JSON schema validation

## Plugin validation

Run Tracktion `pluginval` on the generated VST3/AU. Test: instantiate, editor open/close, state save/restore, MIDI input/output, bypass, sample rate changes, block-size changes, repeated creation/destruction, offline rendering.

## Ableton validation

On Live 12 Suite 12.4.5+ beta, install the Extension, open a test Set, inspect, create a MIDI track, create an audio track, load a native device, load an installed third-party plugin, write a parameter, place an Arrangement clip, render, and verify the returned state.

## Production acceptance

A run is accepted only when the agent can produce an editable arrangement from a prompt using real local samples and real installed devices, then perform an analyze/fix cycle without duplicate mutations or corrupting the Set.
