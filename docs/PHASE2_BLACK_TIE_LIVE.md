# Phase 2 — Black Tie Live

Black Tie Live is the second product and is intentionally separate from Black Tie AI.

## Goal

A low-latency performance MIDI effect that can become the live command/performance hub for:

- MIDI keyboard;
- pad controllers;
- Kaoss Pad-style MIDI devices;
- MIDI clock;
- CC macros;
- quantized scene/clip commands via the Ableton-side integration;
- performance snapshots;
- controller feedback.

## Current implementation

The first performance core provides MIDI pass-through, channel filtering, transpose and MIDI clock generation. It has no network dependency and no AI dependency in its real-time path.

## Planned safe architecture

`MIDI hardware -> Black Tie Live -> Ableton routing/Extension -> tracks/devices`

AI planning remains outside the real-time audio/MIDI callback. The agent can prepare mappings, scenes and performance states, while the callback executes deterministic MIDI transformations.

## Current MIDI macro layer

Eight CC slots are provided in the engine. Incoming controller values are captured into atomic macro state without blocking the audio callback. The next phase can bind these macros to Live parameters, third-party plug-in parameters and controller feedback through the host integration.

## Clock

The engine reads host BPM/play state when supplied and emits MIDI Start/Stop/Clock at 24 PPQN. External clock reception is counted for diagnostics; a future PLL/clock-source mode should be added only after hardware validation, rather than pretending that a simple counter is a robust external-clock synchronizer.
