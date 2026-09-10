#pragma once
#include <JuceHeader.h>

class LocalAudioEngine {
public:
    struct Options {
        float gainDb = 0.0f;
        float highPassHz = 20.0f;
        float lowPassHz = 20000.0f;
        float saturation = 0.0f;
        float fadeInSeconds = 0.0f;
        float fadeOutSeconds = 0.0f;
        int reverse = 0;
        int stutterRepeats = 1;
        float stutterSeconds = 0.0f;
        double targetBpm = 0.0;
        double sourceBpm = 0.0;
    };
    static bool transform(const juce::File& input, const juce::File& output, const Options&, juce::String& error);
};
