#!/bin/bash
# Black Tie AI Producer + Black Tie Live — macOS bootstrap/build
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
if ! command -v xcode-select >/dev/null; then echo "xcode-select is required."; exit 1; fi
if ! xcode-select -p >/dev/null 2>&1; then echo "Apple Command Line Tools are required. macOS requires system approval for this step."; exit 2; fi
if ! command -v git >/dev/null || ! command -v cmake >/dev/null || ! command -v python3 >/dev/null; then
  if ! command -v brew >/dev/null; then /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"; fi
  command -v git >/dev/null || brew install git
  command -v cmake >/dev/null || brew install cmake
  command -v python3 >/dev/null || brew install python
fi
mkdir -p "$ROOT/third_party"
if [ ! -f "$ROOT/third_party/JUCE/CMakeLists.txt" ]; then git clone --depth 1 --branch 9.0.1 https://github.com/juce-framework/JUCE.git "$ROOT/third_party/JUCE"; fi
python3 "$ROOT/scripts/install_all.py"
echo "Done. VST3/AU artifacts are in $ROOT/dist"
