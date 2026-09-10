#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ChatGPTAbletonControlAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Thread
{
public:
    explicit ChatGPTAbletonControlAudioProcessorEditor(ChatGPTAbletonControlAudioProcessor&);
    ~ChatGPTAbletonControlAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ChatGPTAbletonControlAudioProcessor& p;
    juce::Label title, status, providerLabel, strategyLabel, liveLabel;
    juce::TextEditor prompt, output;
    juce::ComboBox provider, strategy;
    juce::TextButton plan{"PLAN"}, generate{"GENERATE"}, execute{"EXECUTE"}, scan{"SCAN"}, live{"LIVE"}, master{"MASTER"}, iterate{"ITERATE"}, stop{"STOP"}, settings{"API / PROVIDERS"};
    juce::Viewport settingsViewport;
    juce::Component settingsPanel;
    struct ProviderRow { juce::ToggleButton enabled; juce::Label name; juce::TextEditor endpoint, model, key; };
    ProviderRow rows[5];
    juce::ToggleButton rememberKeys;
    juce::Label keyWarning;
    juce::Atomic<bool> busy{false};
    juce::String ep, body, result;
    juce::String selected() const;
    void request(const juce::String&, const juce::var&);
    void setBusy(bool);
    void refreshSettingsFromProcessor();
    void saveSettingsToProcessor();
    void toggleSettings();
    void run() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatGPTAbletonControlAudioProcessorEditor)
};
