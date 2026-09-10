#include "ProviderRouter.h"
#include <exception>

ProviderRouter::Result ProviderRouter::call(const Entry& e, const juce::String& system,
                                            const juce::String& user, int timeoutMs)
{
    Result r; r.provider = e.config.name; r.model = e.config.model;
    try { r.json = ProviderClient::chatJSON(e.config, system, user, timeoutMs); r.ok = looksValidObject(r.json); if (!r.ok) r.error = "Provider returned non-object JSON"; }
    catch (const std::exception& ex) { r.error = ex.what(); }
    return r;
}

bool ProviderRouter::looksValidObject(const juce::String& s)
{
    auto v = juce::JSON::parse(s);
    return v.isObject();
}

ProviderRouter::Result ProviderRouter::run(Mode mode, const juce::Array<Entry>& entries,
                                           const juce::String& system, const juce::String& user,
                                           int timeoutMs) const
{
    Result last;
    if (entries.isEmpty()) { last.error = "No AI provider is enabled/configured"; return last; }

    if (mode == Mode::single)
    {
        for (const auto& e : entries)
            if (e.enabled)
                return call(e, system, user, timeoutMs);
        last.error = "No enabled AI provider";
        return last;
    }

    // Ensemble mode is deliberately deterministic: first successful provider generates;
    // subsequent providers receive the candidate and are asked to validate/repair it.
    juce::String candidate;
    for (const auto& e : entries)
    {
        if (!e.enabled) continue;
        const auto prompt = candidate.isEmpty()
            ? user
            : user + "\n\nCANDIDATE JSON FROM ANOTHER PROVIDER:\n" + candidate
              + "\nValidate it against the requested schema. Return a corrected JSON object only.";
        auto r = call(e, system, prompt, timeoutMs);
        if (r.ok) { candidate = r.json; last = r; }
    }
    if (candidate.isNotEmpty()) { last.ok = true; last.json = candidate; return last; }
    last.error = "All enabled providers failed";
    return last;
}
