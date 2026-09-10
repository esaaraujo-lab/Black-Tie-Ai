# Final reference check — 2026-09-10

## Official Ableton

The current Ableton Extensions page states that Extensions can read and rewrite Set structure including tracks and clips, and interact with MIDI, devices and tempo. It also states that Extensions run on Node.js and are distinct from Max for Live. Extensions are currently available in Live 12.4.5 public beta and later compatible beta builds.

https://www.ableton.com/en/live/extensions/
https://help.ableton.com/hc/en-us/articles/27303428331420-Ableton-Extensions-FAQ
https://ableton.github.io/extensions-sdk/

## Official JUCE

`AudioPluginFormatManager::createPluginInstance()` returns a `std::unique_ptr<AudioPluginInstance>`. `PluginDirectoryScanner` is the documented directory scanning class. `URL::InputStreamOptions` provides request method, headers, timeout and redirect configuration. The source was checked against these APIs before this release.

https://docs.juce.com/master/classjuce_1_1AudioPluginFormatManager.html
https://docs.juce.com/master/classjuce_1_1PluginDirectoryScanner.html
https://docs.juce.com/master/classjuce_1_1URL_1_1InputStreamOptions.html
https://docs.juce.com/develop/classjuce_1_1AudioPluginInstance.html

## Community reference projects

https://github.com/ahujasid/ableton-mcp
https://github.com/wstierhout/ableton-live-mcp
https://github.com/idx3d/ableton-mcp-extension
https://github.com/nicholasbien/ableton-mcp-pro
https://github.com/betweentwomidnights/gary4juce
https://github.com/DirtyBeastAfterTheToad/LLMidi
https://github.com/OthmanAdi/loophole
https://github.com/blakebratcher/plugindb
https://github.com/Tracktion/pluginval

## Scope decision

The product's core AI provider layer is independent of Ollama. Ollama is only one optional provider. The VST does not launch, install or require an Ollama process. Direct HTTP API access is used for OpenAI, Gemini and NVIDIA NIM.
