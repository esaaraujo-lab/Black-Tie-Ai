/**
 * Live-side adapter boundary.
 * The exact imports/types are intentionally kept in adapter.ts so the product
 * can track the installed Ableton Extensions SDK without coupling Agent Core to it.
 */
import { createServer } from './server.js';

const server = createServer();
server.start();
