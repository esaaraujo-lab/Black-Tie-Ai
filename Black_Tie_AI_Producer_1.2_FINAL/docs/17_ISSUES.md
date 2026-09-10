# Issue ledger

### I-001 — VST/Live API boundary
Resolved architecturally: Live control is delegated to the official Extension boundary.

### I-002 — Blocking network I/O
Resolved: provider HTTP is called from the editor worker thread; `processBlock` is transparent.

### I-003 — JUCE version drift
Resolved: CMake pins 9.0.1.

### I-004 — Cloud credentials in source
Resolved: keys are runtime state/config only; never hard-coded.

### I-005 — Third-party GUI automation
Resolved conceptually: parameter API first; screen inspection remains fallback.

### I-006 — Binary deployment
Open: requires Windows/macOS native build and signing. Automated GitHub Actions are provided.

### I-007 — Exact Extensions SDK
Open: proprietary SDK must be supplied by Ableton/user. Source scaffold intentionally does not invent SDK calls.
