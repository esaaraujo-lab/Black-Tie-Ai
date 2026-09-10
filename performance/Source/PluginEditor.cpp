#include "PluginEditor.h"

BlackTieLiveAudioProcessorEditor::BlackTieLiveAudioProcessorEditor(BlackTieLiveAudioProcessor& proc) : AudioProcessorEditor(&proc), p(proc)
{
    setSize(560, 300);
    title.setText("BLACK TIE LIVE", juce::dontSendNotification);
    title.setFont(juce::Font(26.0f, juce::Font::bold));
    addAndMakeVisible(title);

    status.setText("Live performance MIDI router / clock engine", juce::dontSendNotification);
    addAndMakeVisible(status);

    clockOut.setButtonText("MIDI Clock Out");
    clockOut.setToggleState(p.getClockOut(), juce::dontSendNotification);
    clockOut.onClick = [this] { p.setClockOut(clockOut.getToggleState()); };
    addAndMakeVisible(clockOut);

    clockIn.setButtonText("MIDI Clock In (future host sync)");
    clockIn.setToggleState(p.getClockIn(), juce::dontSendNotification);
    clockIn.onClick = [this] { p.setClockIn(clockIn.getToggleState()); };
    addAndMakeVisible(clockIn);

    transposeLabel.setText("Transpose", juce::dontSendNotification); addAndMakeVisible(transposeLabel);
    transpose.setRange(-24, 24, 1); transpose.setValue(p.getTranspose()); transpose.setSliderStyle(juce::Slider::LinearHorizontal); transpose.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    transpose.onValueChange = [this] { p.setTranspose(static_cast<int>(transpose.getValue())); }; addAndMakeVisible(transpose);

    channelLabel.setText("MIDI channel (0 = all)", juce::dontSendNotification); addAndMakeVisible(channelLabel);
    channel.setRange(0, 16, 1); channel.setValue(p.getMidiChannel()); channel.setSliderStyle(juce::Slider::LinearHorizontal); channel.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 22);
    channel.onValueChange = [this] { p.setMidiChannel(static_cast<int>(channel.getValue())); }; addAndMakeVisible(channel);

    startTimerHz(5);
}

void BlackTieLiveAudioProcessorEditor::timerCallback()
{
    status.setText(juce::String("Clock ") + (p.getClockOut() ? "ON" : "OFF") + " • Channel " + juce::String(p.getMidiChannel()) + " • Transpose " + juce::String(p.getTranspose()), juce::dontSendNotification);
}

void BlackTieLiveAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0e1116));
    g.setColour(juce::Colour(0xff20252d));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10), 12.0f);
}

void BlackTieLiveAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(28);
    title.setBounds(r.removeFromTop(38));
    status.setBounds(r.removeFromTop(26));
    r.removeFromTop(12);
    clockOut.setBounds(r.removeFromTop(28));
    clockIn.setBounds(r.removeFromTop(28));
    r.removeFromTop(12);
    transposeLabel.setBounds(r.removeFromTop(24));
    transpose.setBounds(r.removeFromTop(30));
    r.removeFromTop(8);
    channelLabel.setBounds(r.removeFromTop(24));
    channel.setBounds(r.removeFromTop(30));
}
