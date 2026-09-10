#pragma once
#include <JuceHeader.h>
#include "ProviderClient.h"
#include "ProviderRouter.h"
#include "ProducerAgent.h"
#include "SampleIndex.h"
#include "PluginHost.h"
#include "LocalAudioEngine.h"

class ChatGPTAbletonControlAudioProcessor final : public juce::AudioProcessor
{
public:
    struct ProviderSlot {
        juce::String name, endpoint, model, apiKey;
        bool enabled = true;
        bool isLocal = false;
    };

    ChatGPTAbletonControlAudioProcessor();
    ~ChatGPTAbletonControlAudioProcessor() override = default;

    void prepareToPlay(double, int) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "ChatGPT × Ableton AI Producer"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    ProviderSlot getProvider(const juce::String&) const;
    void setProvider(const ProviderSlot&);
    void setLiveUrl(const juce::String&);
    juce::String getLiveUrl() const { return liveUrl; }
    void setRouterMode(ProviderRouter::Mode m) { routerMode = m; }
    ProviderRouter::Mode getRouterMode() const { return routerMode; }
    void setRememberKeys(bool b) { rememberKeys = b; }
    bool getRememberKeys() const { return rememberKeys; }

    ProducerAgent::Result plan(const juce::String&, const juce::String&, const juce::String&);
    ProducerAgent::Result masterPlan(const juce::String&, const juce::String&, const juce::String&);
    juce::String liveGet(const juce::String&);
    juce::String livePost(const juce::String&, const juce::String&);

    juce::Array<PluginHost::PluginInfo> installedPlugins() const { return pluginHost.list(); }
    bool scanPlugins(const juce::FileSearchPath& p, juce::String& e) { return pluginHost.scan(p, true, e); }
    bool loadHostedPlugin(const juce::String& n, juce::String& e) { return pluginHost.load(n, e); }
    bool transformSample(const juce::File& in, const juce::File& out, const LocalAudioEngine::Options& o, juce::String& e) { return LocalAudioEngine::transform(in, out, o, e); }

private:
    juce::Array<ProviderRouter::Entry> enabledProviders(const juce::String& preferred = {}) const;
    ProviderConfig configFor(const ProviderSlot&) const;
    juce::String liveUrl{"http://127.0.0.1:9878"};
    ProviderSlot openai{"OpenAI", "https://api.openai.com/v1", "gpt-5.6-luna", {}, true, false};
    ProviderSlot gemini{"Gemini", "https://generativelanguage.googleapis.com/v1beta/openai", "gemini-3.8-flash", {}, true, false};
    ProviderSlot nvidia{"NVIDIA NIM", "https://integrate.api.nvidia.com/v1", "meta/llama-3.3-70b-instruct", {}, true, false};
    ProviderSlot ollamaLocal{"Ollama Local", "http://127.0.0.1:11434/v1", "qwen3:8b", {}, false, true};
    ProviderSlot ollamaCloud{"Ollama Cloud", "https://ollama.com/v1", {}, {}, false, false};
    ProviderRouter::Mode routerMode{ProviderRouter::Mode::single};
    bool rememberKeys = false;
    SampleIndex samples;
    PluginHost pluginHost;
    ProducerAgent agent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatGPTAbletonControlAudioProcessor)
};
