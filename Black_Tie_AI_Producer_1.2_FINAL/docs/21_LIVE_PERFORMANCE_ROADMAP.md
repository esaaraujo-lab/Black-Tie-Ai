# Future live-performance roadmap (reserved architecture)

The final product intentionally reserves interfaces for a later live-set mode without shipping risky realtime control in v1.

## Planned capabilities

- Complete Live Set scene/clip launcher
- MIDI keyboard note, CC and MPE input
- Quantized scene transitions
- MIDI clock / transport synchronization
- Korg Kaoss Pad CC/NRPN/OSC adapter where supported by the hardware
- Macro pages for performance
- Crossfader and send control
- Performance snapshots
- Emergency stop / panic
- CPU-aware plugin activation
- Beat/bar scheduling
- Safe takeover of mapped controls

## Architectural rule

Realtime performance control must never depend on a cloud model round trip. AI can prepare/modify a performance plan, but MIDI clock, note, CC, quantization and emergency-stop paths must remain deterministic and local.
