#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
python3 - <<PY
import json, pathlib, re
r=pathlib.Path("$ROOT")
manifest=json.loads((r/'RELEASE_MANIFEST.json').read_text())
assert manifest['framework']['version']=='9.0.1'
assert 'VST3' in manifest['formats']
for p in ['README.md','THIRD_PARTY_NOTICES.md','docs/19_ATTRIBUTION_AND_LICENSES.md','docs/20_LINE_BY_LINE_REVIEW.md','docs/23_FINAL_PRODUCT_SPEC.md']:
    assert (r/p).exists(), p
print('FINAL AUDIT STATIC: PASS')
PY
