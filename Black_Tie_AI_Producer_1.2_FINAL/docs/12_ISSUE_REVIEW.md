# FINAL 1.1 issue review

## Fixed in this release

- Replaced the old 5.3 JUCE 8.0.15 pin with JUCE 9.0.1.
- Removed blocking HTTP from the audio callback; all requests are made by a GUI worker thread.
- Kept `processBlock` transparent and realtime-safe.
- Added strict localhost/HTTPS endpoint validation.
- Removed the false assumption that a VST can directly access Live's internal object model.
- Moved Live mutations behind a dedicated Extension boundary.
- Added reproducible target-native bootstrap scripts.
- Added Windows/macOS CI builds.
- Kept commercial binaries and Ableton proprietary SDK out of the repository.
- Made local sample indexing independent from the old Node Companion.

## Still requires target validation

- Exact Ableton Extensions SDK API version and packaging.
- VST3/AU compile against JUCE 9.0.1 on Windows/macOS.
- Pluginval on real binaries.
- Live integration on the user's exact Live build.
- Code signing/notarization.

## Design rule

Do not mark a feature “production-ready” merely because a JSON or unit test passes. Host integration must be tested inside Ableton.
