# Third-party plugin strategy

## Why the product hosts third-party plugins itself

Ableton Extensions API 1.0.0 does not expose third-party VST/AU loading, according to the current community capability map for `idx3d/ableton-mcp-extension`. A VST3 also cannot reach sibling devices in the host's track chain through a standard plugin API.

The final product therefore has two distinct paths:

1. **Native Live devices:** loaded/configured through the Live-side adapter.
2. **Third-party VST3/AU:** optionally scanned and hosted *inside the AI Producer plugin* using JUCE's plugin-host APIs.

This means the AI can build its own internal chain such as:

`Third-party synth → EQ → compressor → saturation → limiter`

without pretending that a VST has privileged access to Ableton's private device graph.

The user still needs the third-party plugin installed and properly licensed. The AI Producer does not redistribute commercial plugin binaries.

## Safe hosting

Plugin discovery and instantiation are kept out of `processBlock`. Scanning should be run in a worker/background task and validated with Tracktion `pluginval` before release. A crashing third-party plugin can compromise the host process, so a future hardened scanner process should remain the release recommendation for untrusted plugin collections.
