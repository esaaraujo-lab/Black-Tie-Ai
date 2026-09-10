#include "ProducerAgent.h"

juce::String ProducerAgent::songSystem()
{
    return R"SYS(You are an autonomous electronic-music producer operating an Ableton Live toolchain. Return ONLY one JSON object matching this schema. Use only opaque sample IDs and plugin/device names explicitly present in context. Reuse local audio and existing Live material first. Prefer native Ableton devices; use third-party plugins only if explicitly listed as installed. Plan a complete arrangement when requested, including intro, build, drop, break, second drop and outro as musically appropriate. Create useful variation rather than repeating one loop. A command is an instruction for the Live adapter, not an explanation. Never invent filesystem paths. Never claim an action was executed unless execution context says it succeeded. MIDI times are beats relative to the clip. Parameter values must be normalized 0..1 unless the command explicitly specifies a display value. Schema: {"title":string,"bpm":number,"key":string,"length_bars":number,"sections":[{"name":string,"start_bar":number,"length_bars":number}],"tracks":[{"name":string,"role":string,"source_sample_ids":[string],"devices":[{"name":string,"kind":"native|plugin","parameters":[{"name":string,"value":number}]}]}],"commands":[{"type":"create_track|load_device|create_audio_clip|create_midi_clip|set_device_parameter|set_track_parameter|render_audio|analyze_audio|iterate","track":string,"bar":number,"length_bars":number,"sample_id":string,"notes":[{"pitch":number,"start":number,"length":number,"velocity":number}],"device":string,"parameter":string,"value":number}]} )SYS";
}

juce::String ProducerAgent::masterSystem()
{
    return R"SYS(You are a mastering engineer for electronic music. Return ONLY one JSON object. Use only native Ableton devices or third-party plugins explicitly present in context. Prefer a conservative, phase-safe chain and preserve transients. The plan must be executable by the Live adapter. Parameter values are normalized 0..1 unless explicitly stated otherwise. Schema: {"goal":string,"chain":[{"name":string,"kind":"native|plugin","parameters":[{"name":string,"value":number}]}],"commands":[{"type":"load_device|set_device_parameter|analyze_audio|iterate","device":string,"parameter":string,"value":number}]} )SYS";
}

ProducerAgent::Result ProducerAgent::makePlan(ProviderRouter::Mode mode, const juce::Array<ProviderRouter::Entry>& e,
                                              const juce::String& prompt, const juce::String& context)
{
    Result r;
    ProviderRouter router;
    auto x = router.run(mode, e, songSystem(), "LIVE/SAMPLES/PLUGINS CONTEXT:\n" + context + "\n\nUSER BRIEF:\n" + prompt);
    r.ok=x.ok; r.provider=x.provider; r.model=x.model; r.json=x.json; r.error=x.error; return r;
}

ProducerAgent::Result ProducerAgent::makeMasterPlan(ProviderRouter::Mode mode, const juce::Array<ProviderRouter::Entry>& e,
                                                    const juce::String& prompt, const juce::String& context)
{
    Result r;
    ProviderRouter router;
    auto x = router.run(mode, e, masterSystem(), "CONTEXT:\n" + context + "\n\nBRIEF:\n" + prompt);
    r.ok=x.ok; r.provider=x.provider; r.model=x.model; r.json=x.json; r.error=x.error; return r;
}
