#include "PluginProcessor.h"
#include "PluginEditor.h"

ChatGPTAbletonControlAudioProcessor::ChatGPTAbletonControlAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}

void ChatGPTAbletonControlAudioProcessor::prepareToPlay(double sr, int bs) { pluginHost.prepare(sr, bs); }
void ChatGPTAbletonControlAudioProcessor::releaseResources() { pluginHost.release(); }

bool ChatGPTAbletonControlAudioProcessor::isBusesLayoutSupported(const BusesLayout& x) const
{
    const auto in = x.getMainInputChannelSet(), out = x.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo()) return false;
    return in == out || in == juce::AudioChannelSet::disabled();
}

void ChatGPTAbletonControlAudioProcessor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer& m)
{
    juce::ScopedNoDenormals noDenormals;
    const int inputs = getTotalNumInputChannels(), outputs = getTotalNumOutputChannels();
    for (int c = inputs; c < outputs; ++c) b.clear(c, 0, b.getNumSamples());
    pluginHost.process(b, m);
}

ChatGPTAbletonControlAudioProcessor::ProviderSlot ChatGPTAbletonControlAudioProcessor::getProvider(const juce::String& name) const
{
    const auto n = name.toLowerCase();
    if (n == "gemini") return gemini;
    if (n == "nvidia") return nvidia;
    if (n == "ollama") return ollamaLocal;
    if (n == "ollama_cloud") return ollamaCloud;
    return openai;
}

void ChatGPTAbletonControlAudioProcessor::setProvider(const ProviderSlot& s)
{
    const auto n = s.name.toLowerCase();
    if (n.contains("gemini")) gemini = s;
    else if (n.contains("nvidia")) nvidia = s;
    else if (n.contains("ollama cloud")) ollamaCloud = s;
    else if (n.contains("ollama local")) ollamaLocal = s;
    else openai = s;
}

void ChatGPTAbletonControlAudioProcessor::setLiveUrl(const juce::String& u)
{
    auto s = u.trim();
    if (s.endsWithChar('/')) s = s.dropLastCharacters(1);
    liveUrl = s.isEmpty() ? "http://127.0.0.1:9878" : s;
}

ProviderConfig ChatGPTAbletonControlAudioProcessor::configFor(const ProviderSlot& s) const
{
    ProviderConfig p;
    p.name = s.name; p.baseUrl = s.endpoint; p.model = s.model; p.apiKey = s.apiKey; p.local = s.isLocal;
    return p;
}

juce::Array<ProviderRouter::Entry> ChatGPTAbletonControlAudioProcessor::enabledProviders(const juce::String& preferred) const
{
    juce::Array<ProviderRouter::Entry> out;
    const auto add = [&out, this](const ProviderSlot& s) {
        if (s.enabled && s.endpoint.isNotEmpty() && s.model.isNotEmpty()) out.add({ configFor(s), true });
    };
    if (preferred.isNotEmpty())
    {
        const auto n = preferred.toLowerCase();
        if (n == "openai") { add(openai); return out; }
        if (n == "gemini") { add(gemini); return out; }
        if (n == "nvidia") { add(nvidia); return out; }
        if (n == "ollama") { if (ollamaLocal.enabled) add(ollamaLocal); if (out.isEmpty() && ollamaCloud.enabled) add(ollamaCloud); return out; }
    }
    for (const auto* s : { &openai, &gemini, &nvidia, &ollamaLocal, &ollamaCloud }) add(*s);
    return out;
}

ProducerAgent::Result ChatGPTAbletonControlAudioProcessor::plan(const juce::String& provider, const juce::String& prompt, const juce::String& context)
{
    auto r = agent.makePlan(routerMode, enabledProviders(routerMode == ProviderRouter::Mode::single ? provider : juce::String()), prompt, context);
    return r;
}

ProducerAgent::Result ChatGPTAbletonControlAudioProcessor::masterPlan(const juce::String& provider, const juce::String& prompt, const juce::String& context)
{
    return agent.makeMasterPlan(routerMode, enabledProviders(routerMode == ProviderRouter::Mode::single ? provider : juce::String()), prompt, context);
}

juce::String ChatGPTAbletonControlAudioProcessor::liveGet(const juce::String& e) { return AgentHttp::get(liveUrl + e, 15000); }
juce::String ChatGPTAbletonControlAudioProcessor::livePost(const juce::String& e, const juce::String& j) { return AgentHttp::post(liveUrl + e, j, 60000); }

void ChatGPTAbletonControlAudioProcessor::getStateInformation(juce::MemoryBlock& d)
{
    auto* x = new juce::DynamicObject();
    x->setProperty("liveUrl", liveUrl);
    x->setProperty("routerMode", routerMode == ProviderRouter::Mode::all ? "all" : "single");
    x->setProperty("rememberKeys", rememberKeys);
    juce::Array<juce::var> providers;
    for (const auto* s : { &openai, &gemini, &nvidia, &ollamaLocal, &ollamaCloud }) {
        auto* p = new juce::DynamicObject();
        p->setProperty("name", s->name); p->setProperty("endpoint", s->endpoint); p->setProperty("model", s->model);
        p->setProperty("enabled", s->enabled); p->setProperty("local", s->isLocal);
        if (rememberKeys) p->setProperty("apiKey", s->apiKey);
        providers.add(p);
    }
    x->setProperty("providers", providers);
    juce::MemoryOutputStream out(d, false);
    out.writeString(juce::JSON::toString(juce::var(x)));
}

void ChatGPTAbletonControlAudioProcessor::setStateInformation(const void* data, int size)
{
    const auto text = juce::MemoryInputStream(data, static_cast<size_t>(size), false).readString();
    auto v = juce::JSON::parse(text);
    if (auto* x = v.getDynamicObject()) {
        setLiveUrl(x->getProperty("liveUrl").toString());
        routerMode = x->getProperty("routerMode").toString().equalsIgnoreCase("all") ? ProviderRouter::Mode::all : ProviderRouter::Mode::single;
        rememberKeys = static_cast<bool>(x->getProperty("rememberKeys"));
        if (auto* a = x->getProperty("providers").getArray()) {
            for (const auto& item : *a) if (auto* p = item.getDynamicObject()) {
                ProviderSlot s;
                s.name = p->getProperty("name").toString(); s.endpoint = p->getProperty("endpoint").toString(); s.model = p->getProperty("model").toString();
                s.enabled = static_cast<bool>(p->getProperty("enabled")); s.isLocal = static_cast<bool>(p->getProperty("local"));
                if (rememberKeys) s.apiKey = p->getProperty("apiKey").toString();
                setProvider(s);
            }
        }
    }
}

juce::AudioProcessorEditor* ChatGPTAbletonControlAudioProcessor::createEditor() { return new ChatGPTAbletonControlAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ChatGPTAbletonControlAudioProcessor(); }
