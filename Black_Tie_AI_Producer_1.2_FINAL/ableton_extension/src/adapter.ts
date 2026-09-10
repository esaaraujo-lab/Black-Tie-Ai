/**
 * SDK binding point. Keep this file small and versioned against the installed
 * Ableton Extensions SDK. Agent Core must never import the SDK directly.
 */
export type ExtensionCapabilities = {
  tracks: boolean;
  clips: boolean;
  midi: boolean;
  devices: boolean;
  tempo: boolean;
  arrangement: boolean;
};

export const capabilities: ExtensionCapabilities = {
  tracks: true, clips: true, midi: true, devices: true, tempo: true, arrangement: true
};
