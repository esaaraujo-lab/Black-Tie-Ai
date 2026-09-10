#pragma once
#include <JuceHeader.h>
#include <atomic>
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
    juce::TextButton plan{"PLAN"}, generate{"GENERATE"}, execute{"EXECUTE"}, scan{"SCAN"}, live{"LIVE"}, master{"MASTER"}, iterate{"ITERATE"}, stop{"STOP"}, settings{"SETTINGS"};

    // Idioma: 0 = English (padrao), 1 = Portugues sem acentos (100% ASCII)
    int language = 0;
    juce::TextButton languageBtn{"Language: EN"};
    void retranslate();
    juce::String TR(const char* en, const char* pt) const;

    // Links para criar contas / obter chaves de API
    juce::HyperlinkButton linkOpenAI { "OpenAI: get API key",      juce::URL("https://platform.openai.com/api-keys") };
    juce::HyperlinkButton linkGemini { "Google AI Studio (Gemini)", juce::URL("https://aistudio.google.com/apikey") };
    juce::HyperlinkButton linkNVIDIA { "NVIDIA NIM",                juce::URL("https://build.nvidia.com/") };
    juce::HyperlinkButton linkOllama { "Ollama: download",          juce::URL("https://ollama.com/download") };

    juce::Viewport settingsViewport;
    juce::Component settingsPanel;
    struct ProviderRow { juce::ToggleButton enabled; juce::Label name; juce::TextEditor endpoint, model, key; };
    ProviderRow rows[5];
    juce::ToggleButton rememberKeys;
    juce::Label keyWarning;
    std::atomic<bool> busy{false};
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