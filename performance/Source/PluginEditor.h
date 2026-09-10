#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class BlackTieLiveAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    explicit BlackTieLiveAudioProcessorEditor(BlackTieLiveAudioProcessor&);
    ~BlackTieLiveAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    BlackTieLiveAudioProcessor& p;
    juce::Label title, status;
    juce::ToggleButton clockOut, clockIn;
    juce::Slider transpose, channel;
    juce::Label transposeLabel, channelLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlackTieLiveAudioProcessorEditor)
};
