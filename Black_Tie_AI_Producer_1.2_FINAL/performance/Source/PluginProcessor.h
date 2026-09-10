#pragma once
#include <JuceHeader.h>

class BlackTieLiveAudioProcessor final : public juce::AudioProcessor
{
public:
    BlackTieLiveAudioProcessor();
    ~BlackTieLiveAudioProcessor() override = default;

    void prepareToPlay(double, int) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Black Tie Live"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    void setClockOut(bool) noexcept;
    bool getClockOut() const noexcept { return clockOut.load(); }
    void setClockIn(bool) noexcept;
    bool getClockIn() const noexcept { return clockIn.load(); }
    void setTranspose(int) noexcept;
    int getTranspose() const noexcept { return transpose.load(); }
    void setMidiChannel(int) noexcept;
    int getMidiChannel() const noexcept { return midiChannel.load(); }

    void setMacroCC(int index, int cc) noexcept;
    int getMacroCC(int index) const noexcept;
    void setMacroValue(int index, float value) noexcept;
    float getMacroValue(int index) const noexcept;

private:
    static constexpr int numMacros = 8;
    std::atomic<bool> clockOut { true };
    std::atomic<bool> clockIn { true };
    std::atomic<int> transpose { 0 };
    std::atomic<int> midiChannel { 0 }; // 0 = all channels
    std::atomic<double> tempo { 120.0 };
    std::array<std::atomic<int>, numMacros> macroCC;
    std::array<std::atomic<float>, numMacros> macroValue;
    double sampleRate = 44100.0;
    double samplesUntilClock = 0.0;
    bool clockRunning = false;
    int externalClockTicks = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlackTieLiveAudioProcessor)
};
