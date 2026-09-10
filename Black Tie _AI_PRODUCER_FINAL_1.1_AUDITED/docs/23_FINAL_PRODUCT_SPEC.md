# Final Product Specification

## Goal

The product is a VST3/AU AI producer that can operate as an autonomous production agent while remaining usable as a conventional plugin. The user describes the music in natural language; the agent inspects available material, plans the track, executes reversible operations, and iterates toward a finished arrangement.

## What “complete track” means

A complete generation run is intended to create an editable Live Set structure containing musical sections and tracks rather than a single rendered audio file. Typical output:

- tempo/key and song length;
- intro, build, drop, break, second drop and outro;
- drum, bass, synth, FX and audio/sample tracks;
- MIDI clips with notes;
- audio clips referencing generated derivatives of local material;
- native Ableton devices and installed third-party devices where the Live-side adapter can load them;
- mix and master plan;
- optional render/analyze/iterate cycle.

The agent is allowed to prefer reuse over generation because the user's local library is the fastest and most deterministic source of raw material.

## Offline/online modes

**Cloud:** OpenAI, Gemini or NVIDIA NIM.\n
**Local model:** Ollama, when installed.\n
**Local audio:** transformations run without a model or cloud service.

The VST does not require a Node/Python companion for its core logic.

## Security model

- Cloud credentials stay in plugin configuration and are never sent to the Live adapter.
- Localhost is the default Live bridge address.
- Commands carry unique request IDs.
- The agent must inspect before mutating.
- Mutations should be verified by the Live adapter.
- Timeouts are treated as unknown state, not permission to blindly repeat destructive commands.
- Future live-performance controls must include an independent emergency-stop path.
