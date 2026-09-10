from pathlib import Path
import json, re
root=Path(__file__).resolve().parents[1]
source_ext={'.cpp','.h','.hpp','.ts','.json','.cmake','.ps1','.command','.py'}
files=[p for p in root.rglob('*') if p.is_file() and p.suffix in source_ext and 'third_party' not in p.parts]
assert len(files) >= 30
for required in [
    root/'vst3/Source/PluginProcessor.cpp', root/'vst3/Source/PluginProcessor.h',
    root/'performance/Source/PluginProcessor.cpp', root/'performance/Source/PluginProcessor.h',
    root/'performance/CMakeLists.txt', root/'scripts/install_all.py',
    root/'installer/bootstrap-windows.ps1', root/'installer/bootstrap-macos.command']:
    assert required.exists(), required
text='\n'.join(p.read_text(errors='ignore') for p in files)
assert 'processBlock' in text
assert 'AgentHttp::post' not in (root/'vst3/Source/PluginProcessor.cpp').read_text().split('processBlock',1)[1].split('getProvider',1)[0]
cm=(root/'vst3/CMakeLists.txt').read_text()
assert 'PLUGINHOST_VST3 TRUE' in cm and 'PLUGINHOST_AU TRUE' in cm
pcm=(root/'performance/CMakeLists.txt').read_text()
assert 'IS_MIDI_EFFECT TRUE' in pcm and 'NEEDS_MIDI_OUTPUT TRUE' in pcm
phase=(root/'performance/Source/PluginProcessor.cpp').read_text()
assert 'getPlayHead()' in phase and 'getPosition()' in phase and 'midiClock()' in phase
assert 'AgentHttp' not in phase
proc=(root/'vst3/Source/PluginProcessor.cpp').read_text()
assert 'if (n == "gemini")' in proc and 'if (n == "nvidia")' in proc and 'if (n == "ollama")' in proc
manifest=json.loads((root/'RELEASE_MANIFEST.json').read_text())
assert manifest['version']=='1.2.0'
print(f'FINAL 1.2 SOURCE AUDIT: PASS — {len(files)} files enumerated')
