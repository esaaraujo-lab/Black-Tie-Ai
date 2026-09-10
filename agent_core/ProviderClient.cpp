#include "ProviderClient.h"
#include <stdexcept>

namespace {
juce::String postJSON(const ProviderConfig& p, const juce::String& payload, int timeoutMs)
{
    auto endpoint = p.baseUrl.trimCharactersAtEnd("/") + "/chat/completions";
    juce::URL url(endpoint);
    auto headers = juce::String("Accept: application/json\r\nContent-Type: application/json\r\n")
        + (p.apiKey.isEmpty() ? juce::String() : "Authorization: Bearer " + p.apiKey + "\r\n");
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withHttpRequestCmd("POST")
        .withExtraHeaders(headers)
        .withConnectionTimeoutMs(timeoutMs)
        .withNumRedirectsToFollow(2);
    std::unique_ptr<juce::InputStream> stream(url.withPOSTData(payload).createInputStream(options));
    return stream ? stream->readEntireStreamAsString() : juce::String();
}
}

juce::String ProviderClient::extractText(const juce::var& v)
{
    if (auto* o = v.getDynamicObject()) {
        auto output = o->getProperty("output_text");
        if (output.isString()) return output.toString();
        auto choices = o->getProperty("choices");
        if (auto* a = choices.getArray(); a && !a->isEmpty()) {
            auto* c = a->getReference(0).getDynamicObject();
            if (c) {
                auto msg = c->getProperty("message");
                if (auto* m = msg.getDynamicObject()) {
                    auto content = m->getProperty("content");
                    if (content.isString()) return content.toString();
                    if (auto* parts = content.getArray()) {
                        juce::String s;
                        for (const auto& part : *parts)
                            if (auto* po = part.getDynamicObject()) s += po->getProperty("text").toString();
                        return s;
                    }
                }
            }
        }
    }
    return {};
}

juce::String ProviderClient::cleanJSON(const juce::String& input)
{
    auto s = input.trim();
    if (s.startsWith("```") && s.endsWith("```")) {
        const auto firstNewline = s.indexOfChar('\n');
        if (firstNewline >= 0) s = s.substring(firstNewline + 1, s.length() - 3).trim();
    }
    auto parsed = juce::JSON::parse(s);
    if (parsed.isObject() || parsed.isArray()) return juce::JSON::toString(parsed);
    const auto a = s.indexOfChar('{');
    const auto b = s.lastIndexOfChar('}');
    if (a >= 0 && b > a) {
        parsed = juce::JSON::parse(s.substring(a, b + 1));
        if (parsed.isObject()) return juce::JSON::toString(parsed);
    }
    throw std::runtime_error("Provider did not return valid JSON");
}

juce::String ProviderClient::escapeForError(const juce::String& s)
{
    return s.substring(0, 1000).replace("\n", " ").replace("\r", " ");
}

juce::String ProviderClient::chatJSON(const ProviderConfig& p, const juce::String& system, const juce::String& user, int timeoutMs)
{
    if (!p.local && p.apiKey.isEmpty()) throw std::runtime_error((p.name + ": API key not configured").toStdString());

    auto* root = new juce::DynamicObject();
    root->setProperty("model", p.model);
    root->setProperty("temperature", 0.2);
    juce::Array<juce::var> messages;
    auto* sys = new juce::DynamicObject(); sys->setProperty("role", "system"); sys->setProperty("content", system); messages.add(sys);
    auto* usr = new juce::DynamicObject(); usr->setProperty("role", "user"); usr->setProperty("content", user); messages.add(usr);
    root->setProperty("messages", messages);
    // Do not force provider-specific structured-output fields here: Gemini/OpenAI-compatible
    // gateways differ. The system prompt + strict local JSON validation is the portable baseline.

    const auto raw = postJSON(p, juce::JSON::toString(juce::var(root)), timeoutMs);
    if (raw.isEmpty()) throw std::runtime_error((p.name + ": connection failed").toStdString());
    const auto parsed = juce::JSON::parse(raw);
    if (auto* o = parsed.getDynamicObject()) {
        auto err = o->getProperty("error");
        if (!err.isVoid()) throw std::runtime_error((p.name + ": " + escapeForError(err.toString())).toStdString());
    }
    const auto text = extractText(parsed);
    if (text.isEmpty()) throw std::runtime_error((p.name + ": empty model response").toStdString());
    return cleanJSON(text);
}
