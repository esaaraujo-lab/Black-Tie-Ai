# Deployment reality and zero-install goal

## Can everything be inside the VST3?

Not literally. A VST3 cannot acquire Ableton's private Live Object Model from inside the plugin. The correct minimum-install architecture is a native VST3/AU agent plus a tiny Ableton Extension. The AI, sample index, audio transformations, provider calls and UI are inside the plugin; the Extension is only the host-authorized Live actuator.

## Can the user avoid Node/Python?

Yes for the normal VST runtime. The FINAL 1.1 agent is native C++/JUCE. Node/Python are not required by the VST. The Extension development toolchain is only needed to build the `.ablx`; an installed packaged Extension does not require the developer toolchain.

## Can the agent be like Claude Code?

Yes conceptually: it can maintain a goal, inspect state, call tools, observe results and iterate. The FINAL 1.1 code establishes the in-plugin agent boundary. Full autonomous tool-loop execution belongs in the next integration pass once the exact Live Extension SDK package is available for compilation/testing.
