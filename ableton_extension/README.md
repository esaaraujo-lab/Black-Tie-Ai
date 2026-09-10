# Ableton Live-side adapter

This is the only component outside the VST that needs to know the Ableton Extensions SDK. The agent, provider clients, sample indexing, audio transformation and planning remain in the VST.

The SDK is currently a public beta for Live 12 Suite 12.4.5+. Its exact package/API surface can change with the beta; therefore `src/adapter.ts` is intentionally the versioned binding point. Do not copy proprietary SDK files into this repository.
