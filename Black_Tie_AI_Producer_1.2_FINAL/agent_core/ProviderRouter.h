#pragma once
#include <JuceHeader.h>
#include "ProviderClient.h"

/** Routes structured agent requests to one provider or an ensemble of configured providers.
 *  Ollama is optional: it can target a local OpenAI-compatible endpoint or Ollama Cloud.
 */
class ProviderRouter {
public:
    enum class Mode { single, all };
    struct Entry { ProviderConfig config; bool enabled = true; };
    struct Result { bool ok = false; juce::String provider, model, json, error; };

    Result run(Mode mode, const juce::Array<Entry>& entries,
               const juce::String& system, const juce::String& user,
               int timeoutMs = 240000) const;

private:
    static Result call(const Entry&, const juce::String&, const juce::String&, int);
    static bool looksValidObject(const juce::String&);
};
