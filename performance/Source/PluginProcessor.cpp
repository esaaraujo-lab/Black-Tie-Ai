#include "PluginProcessor.h"
#include "PluginEditor.h"

BlackTieLiveAudioProcessor::BlackTieLiveAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::disabled(), true)
                                      .withOutput("Output", juce::AudioChannelSet::disabled(), true))
{
    for (int i = 0; i < numMacros; ++i)
    {
        macroCC[i].store(20 + i);
        macroValue[i].store(0.0f);
    }
}

void BlackTieLiveAudioProcessor::prepareToPlay(double sr, int)
{
    sampleRate = sr > 0.0 ? sr : 44100.0;
    samplesUntilClock = 0.0;
    externalClockTicks = 0;
    clockRunning = false;
}

void BlackTieLiveAudioProcessor::releaseResources() {}

bool BlackTieLiveAudioProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    return layout.getMainInputChannelSet() == juce::AudioChannelSet::disabled()
        && layout.getMainOutputChannelSet() == juce::AudioChannelSet::disabled();
}

void BlackTieLiveAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    // Read transport from the host when available. JUCE documents PositionInfo as
    // block-start timing information and requires getPosition() to be called here.
    bool hostPlaying = clockRunning;
    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            hostPlaying = pos->getIsPlaying();
            if (auto bpm = pos->getBpm())
                tempo.store(juce::jlimit(20.0, 300.0, *bpm));
        }
    }

    if (hostPlaying != clockRunning)
    {
        clockRunning = hostPlaying;
        samplesUntilClock = 0.0;
        if (clockRunning)
            midi.addEvent(juce::MidiMessage::midiStart(), 0);
        else
            midi.addEvent(juce::MidiMessage::midiStop(), 0);
    }

    // Pass-through MIDI with optional channel filtering, transpose and eight CC macros.
    const int channelFilter = midiChannel.load();
    const int shift = transpose.load();
    juce::MidiBuffer transformed;
    for (const auto metadata : midi)
    {
        auto msg = metadata.getMessage();
        if (clockIn.load() && msg.isMidiClock())
            ++externalClockTicks;

        if (channelFilter > 0 && msg.getChannel() > 0 && msg.getChannel() != channelFilter)
            continue;

        if (shift != 0 && (msg.isNoteOn() || msg.isNoteOff()))
        {
            const int newNote = juce::jlimit(0, 127, msg.getNoteNumber() + shift);
            msg = msg.isNoteOn() ? juce::MidiMessage::noteOn(msg.getChannel(), newNote, msg.getFloatVelocity())
                                 : juce::MidiMessage::noteOff(msg.getChannel(), newNote, msg.getFloatVelocity());
        }

        if (msg.isController())
        {
            const int cc = msg.getControllerNumber();
            for (int i = 0; i < numMacros; ++i)
                if (macroCC[i].load() == cc)
                    macroValue[i].store(msg.getControllerValue() / 127.0f);
        }
        transformed.addEvent(msg, metadata.samplePosition);
    }
    midi.swapWith(transformed);

    // Host-tempo MIDI clock: 24 pulses per quarter note. The DAW remains the
    // transport authority; the plugin supplies clock messages on its MIDI output.
    if (clockOut.load() && clockRunning && sampleRate > 0.0)
    {
        const double bpm = juce::jlimit(20.0, 300.0, tempo.load());
        const double samplesPerClock = sampleRate * 60.0 / (bpm * 24.0);
        const double blockSamples = static_cast<double>(juce::jmax(1, buffer.getNumSamples()));
        double remaining = juce::jmax(1.0, blockSamples);
        while (samplesUntilClock < remaining)
        {
            const int pos = juce::jlimit(0, static_cast<int>(remaining) - 1,
                                          static_cast<int>(std::floor(samplesUntilClock)));
            midi.addEvent(juce::MidiMessage::midiClock(), pos);
            samplesUntilClock += samplesPerClock;
        }
        samplesUntilClock -= remaining;
    }
}

void BlackTieLiveAudioProcessor::setClockOut(bool b) noexcept { clockOut.store(b); }
void BlackTieLiveAudioProcessor::setClockIn(bool b) noexcept { clockIn.store(b); }
void BlackTieLiveAudioProcessor::setTranspose(int v) noexcept { transpose.store(juce::jlimit(-24, 24, v)); }
void BlackTieLiveAudioProcessor::setMidiChannel(int v) noexcept { midiChannel.store(juce::jlimit(0, 16, v)); }
void BlackTieLiveAudioProcessor::setMacroCC(int index, int cc) noexcept { if (juce::isPositiveAndBelow(index, numMacros)) macroCC[index].store(juce::jlimit(0, 127, cc)); }
int BlackTieLiveAudioProcessor::getMacroCC(int index) const noexcept { return juce::isPositiveAndBelow(index, numMacros) ? macroCC[index].load() : -1; }
void BlackTieLiveAudioProcessor::setMacroValue(int index, float value) noexcept { if (juce::isPositiveAndBelow(index, numMacros)) macroValue[index].store(juce::jlimit(0.0f, 1.0f, value)); }
float BlackTieLiveAudioProcessor::getMacroValue(int index) const noexcept { return juce::isPositiveAndBelow(index, numMacros) ? macroValue[index].load() : 0.0f; }

void BlackTieLiveAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto* root = new juce::DynamicObject();
    root->setProperty("clockOut", clockOut.load()); root->setProperty("clockIn", clockIn.load());
    root->setProperty("transpose", transpose.load()); root->setProperty("midiChannel", midiChannel.load());
    juce::Array<juce::var> macros;
    for (int i = 0; i < numMacros; ++i) { auto* m = new juce::DynamicObject(); m->setProperty("cc", macroCC[i].load()); m->setProperty("value", macroValue[i].load()); macros.add(m); }
    root->setProperty("macros", macros);
    juce::MemoryOutputStream out(dest, false); out.writeString(juce::JSON::toString(juce::var(root)));
}

void BlackTieLiveAudioProcessor::setStateInformation(const void* data, int size)
{
    auto text = juce::MemoryInputStream(data, static_cast<size_t>(size), false).readString();
    auto v = juce::JSON::parse(text);
    if (auto* root = v.getDynamicObject())
    {
        setClockOut(static_cast<bool>(root->getProperty("clockOut"))); setClockIn(static_cast<bool>(root->getProperty("clockIn")));
        setTranspose(static_cast<int>(root->getProperty("transpose"))); setMidiChannel(static_cast<int>(root->getProperty("midiChannel")));
        if (auto* a = root->getProperty("macros").getArray()) for (int i = 0; i < juce::jmin(numMacros, a->size()); ++i)
            if (auto* m = a->getReference(i).getDynamicObject()) { setMacroCC(i, static_cast<int>(m->getProperty("cc"))); setMacroValue(i, static_cast<float>(m->getProperty("value"))); }
    }
}

juce::AudioProcessorEditor* BlackTieLiveAudioProcessor::createEditor() { return new BlackTieLiveAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new BlackTieLiveAudioProcessor(); }

