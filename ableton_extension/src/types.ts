export type AgentCommand = { requestId: string; type: string; [key:string]: unknown };
export type LiveState = { tempo?: number; tracks?: unknown[]; devices?: unknown[]; clips?: unknown[] };
