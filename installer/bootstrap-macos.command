#!/bin/bash
# Black Tie AI Producer + Black Tie Live — macOS installer/build
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
echo ""
echo "=== BLACK TIE AI PRODUCER / BLACK TIE LIVE ==="
echo "Starting unattended bootstrap and build..."
PY="$(command -v python3 || true)"
if [[ -z "$PY" ]]; then echo "ERROR: python3 is required."; exit 1; fi
exec "$PY" "$ROOT/install.py" "$@"
