# Release certification checklist

The source package is final in the sense of architecture and implementation scope, but native binaries require the target toolchains and Ableton installation.

## Windows

1. Run `installer/bootstrap-windows.ps1`.
2. Run `pluginval` against the generated VST3.
3. Install the VST3 in the standard VST3 directory.
4. Open Ableton Live 12 Suite and run the Extension smoke tests.
5. Test local sample scan, MIDI generation, hosted VST3 loading, native-device commands, arrangement creation, save/reload, and emergency stop.

## macOS

1. Run `installer/bootstrap-macos.command`.
2. Run `pluginval` against VST3 and AU.
3. Test Apple Silicon and Intel only if both are in the release target.
4. Code-sign and notarize release binaries.
5. Run the same Live smoke suite.

## Do not ship as certified until

- JUCE build is green on both targets;
- pluginval is green;
- Ableton Extension self-test is green on the target Live build;
- a real third-party VST3 has been loaded into the internal host and rendered;
- a real user sample has been transformed and inserted;
- an end-to-end complete-track prompt has been executed;
- a failed provider request does not corrupt the set;
- credentials are not written into logs or project files.
