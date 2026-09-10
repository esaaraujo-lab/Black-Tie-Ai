# Reproducible JUCE clone

The build is pinned to JUCE 9.0.1:

`git clone --depth 1 --branch 9.0.1 https://github.com/juce-framework/JUCE.git third_party/JUCE`

Then CMake points to that checkout. This is preferable to an unpinned master branch because plugin builds need deterministic framework versions.

## Current execution environment

The command was attempted during this build:

`git ls-remote https://github.com/juce-framework/JUCE.git HEAD`

The environment returned a DNS resolution failure for `github.com`. Therefore no source was downloaded and no target-native VST3/AU binary was fabricated.
