#pragma once
#include <JuceHeader.h>

struct ProviderConfig {
    juce::String name;
    juce::String baseUrl;
    juce::String model;
    juce::String apiKey;
    bool local = false;
};

class ProviderClient {
public:
    static juce::String chatJSON(const ProviderConfig&, const juce::String&, const juce::String&, int timeoutMs = 240000);
private:
    static juce::String extractText(const juce::var&);
    static juce::String cleanJSON(const juce::String&);
    static juce::String escapeForError(const juce::String&);
};
