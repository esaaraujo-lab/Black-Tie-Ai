$ErrorActionPreference='Stop';$Root=(Resolve-Path "$PSScriptRoot\..").Path;$Juce="$Root\third_party\JUCE";New-Item -ItemType Directory -Force "$Root\third_party"|Out-Null
if(!(Test-Path "$Juce\CMakeLists.txt")){git clone --depth 1 --branch 9.0.1 https://github.com/juce-framework/JUCE.git $Juce}
cmake -S "$Root\vst3" -B "$Root\build-win" -G "Visual Studio 17 2022" -A x64 -DJUCE_SOURCE_DIR="$Juce"
cmake --build "$Root\build-win" --config Release --target ChatGPTAbletonControl_VST3
New-Item -ItemType Directory -Force "$Root\dist"|Out-Null;Copy-Item -Recurse "$Root\build-win\ChatGPTAbletonControl_artefacts\Release\VST3\*.vst3" "$Root\dist\" -Force
Write-Host "Built: $Root\dist"
