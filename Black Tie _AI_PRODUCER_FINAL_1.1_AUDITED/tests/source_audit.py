from pathlib import Path
import json, re
root=Path(__file__).resolve().parents[1]
files=[*root.glob('agent_core/*.cpp'),*root.glob('agent_core/*.h'),*root.glob('vst3/Source/*.cpp'),*root.glob('vst3/Source/*.h')]
text='\n'.join(f.read_text(errors='ignore') for f in files)
cm=(root/'vst3/CMakeLists.txt').read_text()
proc=(root/'vst3/Source/PluginProcessor.cpp').read_text()
editor=(root/'vst3/Source/PluginEditor.cpp').read_text()
host=(root/'agent_core/PluginHost.cpp').read_text()
client=(root/'agent_core/ProviderClient.cpp').read_text()
assert 'ProviderRouter.cpp' in cm
assert 'createPluginInstance(chosen' in host
assert 'instance.release()' in host
assert 'ScopedTryLock' in host
assert 'param->getParameterID()' in host
assert 'response_format' not in client
for token in ['openai','gemini','nvidia','ollamaLocal','ollamaCloud','rememberKeys','routerMode']:
    assert token in proc, token
assert 'MessageManager::callAsync' in editor
assert 'startThread' in editor
assert 'processBlock' in text
assert 'AgentHttp::post' not in text.split('void ChatGPTAbletonControlAudioProcessor::processBlock',1)[1].split('ProviderSlot',1)[0]
assert (root/'docs/19_ATTRIBUTION_AND_LICENSES.md').exists()
assert (root/'docs/20_LINE_BY_LINE_REVIEW.md').exists()
assert (root/'docs/30_PROVIDER_CONFIGURATION.md').exists()
manifest=json.loads((root/'RELEASE_MANIFEST.json').read_text())
assert manifest['version']=='1.1.0'
assert manifest['providers']==['OpenAI','Gemini','NVIDIA NIM','Ollama']
print(f'source audit: PASS ({len(files)} source files inspected)')
