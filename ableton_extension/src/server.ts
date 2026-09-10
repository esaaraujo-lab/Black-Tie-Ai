import { randomUUID } from 'node:crypto';
import type { AgentCommand, LiveState } from './types.js';

/**
 * Transport-neutral adapter. Bind the functions in adapter.ts to the exact
 * Ableton Extensions SDK API shipped with the user's Live beta/build.
 */
export type LiveAdapter = {
  inspect: () => Promise<LiveState>;
  execute: (command: AgentCommand) => Promise<unknown>;
  emergencyStop: () => Promise<void>;
};

export function createServer(adapter?: LiveAdapter) {
  const id = randomUUID();
  return {
    id,
    start() { /* SDK host registration is performed by the packaged Extension runtime. */ },
    async inspect() { return adapter ? adapter.inspect() : { tracks: [] }; },
    async execute(command: AgentCommand) {
      if (!command.requestId) throw new Error('requestId is required');
      return adapter ? adapter.execute(command) : { ok: false, error: 'Live adapter not bound' };
    },
    async emergencyStop() { if (adapter) await adapter.emergencyStop(); }
  };
}
