# Reserved interfaces for live performance

The final product keeps MIDI I/O enabled in the plugin and isolates future performance logic from the composition agent.

Planned later module:

`Performance Engine → MIDI Keyboard / MIDI Clock / CC / MPE → Live Extension → scenes/clips/devices`

A Kaoss Pad adapter can later map its supported MIDI CC/NRPN/OSC controls to named performance macros. The realtime path must not depend on GPT/Gemini/NVIDIA/Ollama response latency.

This roadmap is intentionally not enabled in the final release profile until the production workflow is tested.
