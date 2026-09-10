#include "PluginEditor.h"

ChatGPTAbletonControlAudioProcessorEditor::ChatGPTAbletonControlAudioProcessorEditor(ChatGPTAbletonControlAudioProcessor& x)
    : AudioProcessorEditor(&x), Thread("AI Producer request"), p(x)
{
    setSize(1180, 820);
    title.setText("CHATGPT × ABLETON AI PRODUCER — FINAL", juce::dontSendNotification);
    title.setFont(juce::Font(21.0f, juce::Font::bold)); addAndMakeVisible(title);
    status.setText("Agent embedded • configure providers below • Ollama is optional", juce::dontSendNotification); addAndMakeVisible(status);

    prompt.setMultiLine(true); prompt.setReturnKeyStartsNewLine(true);
    prompt.setTextToShowWhenEmpty("Crie uma música completa usando meus loops/samples. Analise o material disponível, crie variações, escolha instrumentos/efeitos nativos e plugins de terceiros instalados, monte o Arrangement e faça mix/master.", juce::Colours::grey);
    addAndMakeVisible(prompt);

    provider.addItem("OpenAI GPT", 1); provider.addItem("Google Gemini", 2); provider.addItem("NVIDIA NIM", 3); provider.addItem("Ollama", 4); provider.addItem("Todos (ensemble)", 5);
    provider.setSelectedId(1); addAndMakeVisible(provider);
    strategy.addItem("1 API", 1); strategy.addItem("Todas as APIs", 2); strategy.setSelectedId(p.getRouterMode() == ProviderRouter::Mode::all ? 2 : 1); addAndMakeVisible(strategy);

    for (auto* b : { &plan,&generate,&execute,&scan,&live,&master,&iterate,&stop,&settings }) addAndMakeVisible(b);
    output.setMultiLine(true); output.setReadOnly(true); output.setScrollbarsShown(true); output.setCaretVisible(false); addAndMakeVisible(output);

    provider.onChange = [this] { if (provider.getSelectedId() == 5) strategy.setSelectedId(2); };
    strategy.onChange = [this] { p.setRouterMode(strategy.getSelectedId() == 2 ? ProviderRouter::Mode::all : ProviderRouter::Mode::single); };
    plan.onClick = [this] { request("plan", {}); };
    generate.onClick = [this] { request("plan", {}); };
    execute.onClick = [this] {
        if (result.isEmpty()) { output.setText("Generate a valid plan first.", false); return; }
        auto parsed = juce::JSON::parse(result);
        if (!parsed.isObject()) { output.setText("The current plan is not a valid JSON object.", false); return; }
        request("execute", parsed);
    };
    scan.onClick = [this] { request("scan", {}); };
    live.onClick = [this] { request("live", {}); };
    master.onClick = [this] { request("master", {}); };
    iterate.onClick = [this] { request("iterate", {}); };
    stop.onClick = [this] { request("stop", {}); };
    settings.onClick = [this] { toggleSettings(); };

    settingsPanel.setVisible(false);
    settingsPanel.addAndMakeVisible(rememberKeys);
    rememberKeys.setButtonText("Save API keys in the Ableton project state");
    rememberKeys.setToggleState(p.getRememberKeys(), juce::dontSendNotification);
    keyWarning.setText("Warning: saving keys embeds them in the plugin/project state. Leave off when possible.", juce::dontSendNotification);
    keyWarning.setColour(juce::Label::textColourId, juce::Colours::orange);
    settingsPanel.addAndMakeVisible(keyWarning);
    settingsViewport.setViewedComponent(&settingsPanel, false);
    settingsViewport.setVisible(false);
    addAndMakeVisible(settingsViewport);
    refreshSettingsFromProcessor();
}

ChatGPTAbletonControlAudioProcessorEditor::~ChatGPTAbletonControlAudioProcessorEditor() { stopThread(10000); saveSettingsToProcessor(); }

juce::String ChatGPTAbletonControlAudioProcessorEditor::selected() const
{
    switch (provider.getSelectedId()) { case 2: return "gemini"; case 3: return "nvidia"; case 4: return "ollama"; default: return "openai"; }
}

void ChatGPTAbletonControlAudioProcessorEditor::refreshSettingsFromProcessor()
{
    const juce::String names[5] = { "OpenAI", "Gemini", "NVIDIA NIM", "Ollama Local", "Ollama Cloud" };
    for (int i = 0; i < 5; ++i) {
        auto s = p.getProvider(names[i]);
        rows[i].name.setText(names[i], juce::dontSendNotification);
        rows[i].enabled.setButtonText("Enabled"); rows[i].enabled.setToggleState(s.enabled, juce::dontSendNotification);
        rows[i].endpoint.setText(s.endpoint, false); rows[i].model.setText(s.model, false); rows[i].key.setText(s.apiKey, false); rows[i].key.setPasswordCharacter('*');
        settingsPanel.addAndMakeVisible(rows[i].enabled); settingsPanel.addAndMakeVisible(rows[i].name); settingsPanel.addAndMakeVisible(rows[i].endpoint); settingsPanel.addAndMakeVisible(rows[i].model); settingsPanel.addAndMakeVisible(rows[i].key);
    }
}

void ChatGPTAbletonControlAudioProcessorEditor::saveSettingsToProcessor()
{
    const juce::String names[5] = { "OpenAI", "Gemini", "NVIDIA NIM", "Ollama Local", "Ollama Cloud" };
    for (int i = 0; i < 5; ++i) {
        auto s = p.getProvider(names[i]); s.name = names[i]; s.enabled = rows[i].enabled.getToggleState();
        s.endpoint = rows[i].endpoint.getText().trim(); s.model = rows[i].model.getText().trim(); s.apiKey = rows[i].key.getText();
        p.setProvider(s);
    }
    p.setRememberKeys(rememberKeys.getToggleState());
    p.setRouterMode(strategy.getSelectedId() == 2 ? ProviderRouter::Mode::all : ProviderRouter::Mode::single);
}

void ChatGPTAbletonControlAudioProcessorEditor::toggleSettings()
{
    const bool visible = !settingsViewport.isVisible();
    if (visible) refreshSettingsFromProcessor(); else saveSettingsToProcessor();
    settingsPanel.setVisible(visible); settingsViewport.setVisible(visible); resized();
}

void ChatGPTAbletonControlAudioProcessorEditor::setBusy(bool b)
{
    busy.store(b);
    juce::MessageManager::callAsync([this, b] {
        for (auto* x : { &plan,&generate,&execute,&scan,&live,&master,&iterate,&stop,&settings }) x->setEnabled(!b || x == &stop);
        status.setText(b ? "Agent working…" : "Agent ready", juce::dontSendNotification);
    });
}

void ChatGPTAbletonControlAudioProcessorEditor::request(const juce::String& e, const juce::var& v)
{
    if (busy.load() || isThreadRunning()) return;
    saveSettingsToProcessor();
    ep = e; body = juce::JSON::toString(v); result.clear(); setBusy(true); startThread();
}

void ChatGPTAbletonControlAudioProcessorEditor::run()
{
    juce::String r;
    if (ep == "plan" || ep == "iterate") {
        auto rr = p.plan(selected(), prompt.getText(), "Request the Live-side adapter to inspect current tracks/clips/devices. Use only indexed local samples and installed/native devices. Ollama may be unavailable; do not require it.");
        r = rr.ok ? rr.json : rr.error; if (ep == "plan" && rr.ok) result = rr.json;
    } else if (ep == "master") {
        auto rr = p.masterPlan(selected(), prompt.getText(), "Use only installed/native devices reported by the Live-side adapter."); r = rr.ok ? rr.json : rr.error;
    } else if (ep == "live") r = p.liveGet("/health");
    else if (ep == "execute") r = p.livePost("/execute", body);
    else if (ep == "scan") r = p.livePost("/samples/scan", "{}");
    else if (ep == "stop") r = p.livePost("/stop", "{}");
    else r = "Unknown action";
    juce::MessageManager::callAsync([this, r] { output.setText(r, false); setBusy(false); });
}

void ChatGPTAbletonControlAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0e1116)); g.setColour(juce::Colour(0xff20252d)); g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10), 12.0f);
}

void ChatGPTAbletonControlAudioProcessorEditor::resized()
{
    auto a = getLocalBounds().reduced(24);
    title.setBounds(a.removeFromTop(38)); status.setBounds(a.removeFromTop(24));
    prompt.setBounds(a.removeFromTop(settingsViewport.isVisible() ? 125 : 185)); a.removeFromTop(10);
    auto r = a.removeFromTop(34);
    provider.setBounds(r.removeFromLeft(150)); r.removeFromLeft(8); strategy.setBounds(r.removeFromLeft(120)); r.removeFromLeft(8);
    plan.setBounds(r.removeFromLeft(55)); r.removeFromLeft(6); generate.setBounds(r.removeFromLeft(95)); r.removeFromLeft(6); execute.setBounds(r.removeFromLeft(75)); r.removeFromLeft(6); scan.setBounds(r.removeFromLeft(65)); r.removeFromLeft(6); live.setBounds(r.removeFromLeft(60)); r.removeFromLeft(6); settings.setBounds(r.removeFromLeft(145));
    a.removeFromTop(8); auto r2 = a.removeFromTop(34); master.setBounds(r2.removeFromLeft(80)); r2.removeFromLeft(8); iterate.setBounds(r2.removeFromLeft(80)); r2.removeFromLeft(8); stop.setBounds(r2.removeFromLeft(70));
    if (settingsViewport.isVisible()) {
        a.removeFromTop(8); auto sr = a.removeFromTop(205); settingsViewport.setBounds(sr);
        settingsPanel.setSize(sr.getWidth() - 18, 205);
        auto top = settingsPanel.getLocalBounds().reduced(8); rememberKeys.setBounds(top.removeFromTop(24)); keyWarning.setBounds(top.removeFromTop(24)); top.removeFromTop(4);
        const int col[5] = { 100, 330, 260, 250, 300 }; juce::ignoreUnused(col);
        for (int i = 0; i < 5; ++i) { auto row = top.removeFromTop(30); rows[i].enabled.setBounds(row.removeFromLeft(75)); row.removeFromLeft(5); rows[i].name.setBounds(row.removeFromLeft(100)); row.removeFromLeft(5); rows[i].endpoint.setBounds(row.removeFromLeft(300)); row.removeFromLeft(5); rows[i].model.setBounds(row.removeFromLeft(220)); row.removeFromLeft(5); rows[i].key.setBounds(row); }
    }
    a.removeFromTop(10); output.setBounds(a);
}
