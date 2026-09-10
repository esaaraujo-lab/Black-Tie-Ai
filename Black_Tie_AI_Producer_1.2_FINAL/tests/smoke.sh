#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
python3 "$ROOT/tests/source_audit.py"
python3 - <<PY
import json
from pathlib import Path
r=Path("$ROOT")
json.loads((r/'RELEASE_MANIFEST.json').read_text())
print('manifest: PASS')
PY
