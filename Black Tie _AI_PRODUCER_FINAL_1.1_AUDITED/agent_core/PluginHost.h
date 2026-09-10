#pragma once
#include <JuceHeader.h>

/** Hosts third-party VST3/AU plugins inside the AI Producer itself.
 * This is deliberately independent of Ableton's sibling-device chain.
 */
class PluginHost : public juce::AudioProcessorListener
{
public:
    struct PluginInfo { juce::String name, vendor, format, file; };
    PluginHost();
    ~PluginHost() override;

    void prepare(double sampleRate, int maxBlockSize);
    void release();
    void process(juce::AudioBuffer<float>&, juce::MidiBuffer&);

    bool scan(const juce::FileSearchPath&, bool recursive, juce::String& error);
    juce::Array<PluginInfo> list() const;
    juce::Array<PluginInfo> search(const juce::String&, int maxResults = 20) const;
    bool load(const juce::String& nameOrPath, juce::String& error);
    bool setParameter(const juce::String& parameterName, float normalized, juce::String& error);
    juce::var state() const;

private:
    juce::AudioPluginFormatManager formats;
    juce::KnownPluginList known;
    juce::OwnedArray<juce::AudioPluginInstance> chain;
    double rate = 44100.0;
    int block = 512;
    mutable juce::CriticalSection mutex;
    void audioProcessorParameterChanged(juce::AudioProcessor*, int, float) override {}
    void audioProcessorChanged(juce::AudioProcessor*, const juce::AudioProcessorListener::ChangeDetails&) override {}
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginHost)
};
