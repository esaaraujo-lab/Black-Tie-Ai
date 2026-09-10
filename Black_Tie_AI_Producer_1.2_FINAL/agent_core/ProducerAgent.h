#pragma once
#include <JuceHeader.h>
#include "ProviderRouter.h"

class ProducerAgent {
public:
    struct Result { bool ok=false; juce::String provider; juce::String model; juce::String json; juce::String error; };
    Result makePlan(ProviderRouter::Mode, const juce::Array<ProviderRouter::Entry>&, const juce::String&, const juce::String&);
    Result makeMasterPlan(ProviderRouter::Mode, const juce::Array<ProviderRouter::Entry>&, const juce::String&, const juce::String&);
private:
    static juce::String songSystem();
    static juce::String masterSystem();
};
