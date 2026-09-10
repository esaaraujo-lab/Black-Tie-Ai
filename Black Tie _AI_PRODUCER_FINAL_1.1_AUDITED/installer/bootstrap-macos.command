#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.."&&pwd)";mkdir -p "$ROOT/third_party";if [ ! -f "$ROOT/third_party/JUCE/CMakeLists.txt" ];then git clone --depth 1 --branch 9.0.1 https://github.com/juce-framework/JUCE.git "$ROOT/third_party/JUCE";fi
cmake -S "$ROOT/vst3" -B "$ROOT/build-mac" -DCMAKE_BUILD_TYPE=Release -DJUCE_SOURCE_DIR="$ROOT/third_party/JUCE"
cmake --build "$ROOT/build-mac" --config Release --target ChatGPTAbletonControl_VST3 ChatGPTAbletonControl_AU
mkdir -p "$ROOT/dist";cp -R "$ROOT/build-mac/ChatGPTAbletonControl_artefacts/Release/VST3/"*.vst3 "$ROOT/dist/";for x in "$ROOT/build-mac/ChatGPTAbletonControl_artefacts/Release/AU/"*.component; do [ -d "$x" ] && cp -R "$x" "$ROOT/dist/" || true; done
echo "Built: $ROOT/dist"
