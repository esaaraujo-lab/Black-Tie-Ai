#include "ProviderClient.h"
#include <stdexcept>

namespace {

juce::var makeTextPart(const juce::String& t)
{
    auto* o = new juce::DynamicObject();
    o->setProperty("text", t);
    return juce::var(o);
}

juce::String postJSON(const ProviderConfig& p, const juce::String& urlStr,
                      const juce::String& payload, int timeoutMs, bool useBearer)
{
    juce::String headers = "Accept: application/json\r\nContent-Type: application/json\r\n";
    if (p.apiKey.isNotEmpty())
        headers += useBearer ? ("Authorization: Bearer " + p.apiKey + "\r\n")
                             : ("x-goog-api-key: " + p.apiKey + "\r\n");
    juce::URL url(urlStr);
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withHttpRequestCmd("POST")
        .withExtraHeaders(headers)
        .withConnectionTimeoutMs(timeoutMs)
        .withNumRedirectsToFollow(2);
    std::unique_ptr<juce::InputStream> stream(url.withPOSTData(payload).createInputStream(options));
    return stream ? stream->readEntireStreamAsString() : juce::String();
}

// Extrai a mensagem real de um campo "error" (objeto {"message":...}, string, ou array)
juce::String errorMessageFrom(const juce::var& parsed)
{
    auto probe = parsed;
    if (probe.isArray() && probe.size() > 0) probe = probe[0];  // gateways que embrulham em array
    if (auto* o = probe.getDynamicObject())
    {
        auto err = o->getProperty("error");
        if (!err.isVoid())
        {
            if (auto* eo = err.getDynamicObject())
            {
                auto msg = eo->getProperty("message").toString();
                if (msg.isEmpty()) msg = juce::JSON::toString(err);
                return msg;
            }
            if (err.isString()) return err.toString();
            return juce::JSON::toString(err);
        }
    }
    return {};
}

} // namespace

juce::String ProviderClient::extractText(const juce::var& v)
{
    if (v.isArray() && v.size() > 0) return extractText(v[0]);  // tolera array no topo

    if (auto* o = v.getDynamicObject())
    {
        auto output = o->getProperty("output_text");
        if (output.isString() && output.toString().isNotEmpty()) return output.toString();

        auto choices = o->getProperty("choices");
        if (auto* a = choices.getArray(); a && !a->isEmpty())
            if (auto* c = a->getReference(0).getDynamicObject())
            {
                auto msg = c->getProperty("message");
                if (auto* m = msg.getDynamicObject())
                {
                    auto content = m->getProperty("content");
                    if (content.isString() && content.toString().isNotEmpty()) return content.toString();
                    if (auto* parts = content.getArray())
                    {
                        juce::String s;
                        for (const auto& part : *parts)
                            if (auto* po = part.getDynamicObject())
                            {
                                auto th = po->getProperty("thought");
                                if (th.isBool() && (bool) th) continue;
                                s += po->getProperty("text").toString();
                            }
                        if (s.isNotEmpty()) return s;
                    }
                    auto rc = m->getProperty("reasoning_content");
                    if (rc.isString() && rc.toString().isNotEmpty()) return rc.toString();
                }
            }

        auto out = o->getProperty("output");
        if (auto* oa = out.getArray())
        {
            juce::String s;
            for (const auto& item : *oa)
                if (auto* io = item.getDynamicObject())
                    if (auto* ca = io->getProperty("content").getArray())
                        for (const auto& cp : *ca)
                            if (auto* cpo = cp.getDynamicObject())
                            {
                                auto t = cpo->getProperty("text");
                                if (t.isString()) s += t.toString();
                                else if (auto* to = t.getDynamicObject()) s += to->getProperty("value").toString();
                            }
            if (s.isNotEmpty()) return s;
        }

        // Formato NATIVO do Gemini: candidates[0].content.parts[].text
        auto candidates = o->getProperty("candidates");
        if (auto* ca = candidates.getArray(); ca && !ca->isEmpty())
            if (auto* c0 = ca->getReference(0).getDynamicObject())
                if (auto* co = c0->getProperty("content").getDynamicObject())
                    if (auto* pa = co->getProperty("parts").getArray())
                    {
                        juce::String s;
                        for (const auto& part : *pa)
                            if (auto* po = part.getDynamicObject())
                            {
                                auto th = po->getProperty("thought");
                                if (th.isBool() && (bool) th) continue;
                                s += po->getProperty("text").toString();
                            }
                        if (s.isNotEmpty()) return s;
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

    // Gemini nativo: a camada OpenAI-compat do generativelanguage nao aceita as
    // chaves novas (formato AQ.*) — retorna 404. O protocolo nativo aceita.
    const bool nativeGemini = p.baseUrl.contains("generativelanguage.googleapis.com")
                           && !p.baseUrl.contains("/openai");

    juce::String raw;
    if (nativeGemini)
    {
        const auto urlStr = p.baseUrl.trimCharactersAtEnd("/")
                          + "/models/" + p.model.trim() + ":generateContent";

        auto* sysObj = new juce::DynamicObject();
        juce::Array<juce::var> sysParts; sysParts.add(makeTextPart(system));
        sysObj->setProperty("parts", sysParts);

        auto* turn = new juce::DynamicObject();
        turn->setProperty("role", "user");
        juce::Array<juce::var> parts; parts.add(makeTextPart(user));
        turn->setProperty("parts", parts);

        auto* genCfg = new juce::DynamicObject();
        genCfg->setProperty("temperature", 0.2);

        auto* root = new juce::DynamicObject();
        root->setProperty("systemInstruction", juce::var(sysObj));
        root->setProperty("contents", juce::Array<juce::var>{ turn });
        root->setProperty("generationConfig", juce::var(genCfg));

        raw = postJSON(p, urlStr, juce::JSON::toString(juce::var(root)), timeoutMs, /*useBearer*/ false);
    }
    else
    {
        const auto urlStr = p.baseUrl.trimCharactersAtEnd("/") + "/chat/completions";

        auto* root = new juce::DynamicObject();
        root->setProperty("model", p.model);
        root->setProperty("temperature", 0.2);
        juce::Array<juce::var> messages;
        auto* sys = new juce::DynamicObject(); sys->setProperty("role", "system"); sys->setProperty("content", system); messages.add(sys);
        auto* usr = new juce::DynamicObject(); usr->setProperty("role", "user"); usr->setProperty("content", user); messages.add(usr);
        root->setProperty("messages", messages);

        raw = postJSON(p, urlStr, juce::JSON::toString(juce::var(root)), timeoutMs, /*useBearer*/ true);
    }

    if (raw.isEmpty()) throw std::runtime_error((p.name + ": connection failed (HTTP error or timeout)").toStdString());

    const auto parsed = juce::JSON::parse(raw);
    const auto errMsg = errorMessageFrom(parsed);
    if (errMsg.isNotEmpty())
        throw std::runtime_error((p.name + ": " + escapeForError(errMsg)).toStdString());

    const auto text = extractText(parsed);
    if (text.isEmpty()) throw std::runtime_error((p.name + ": empty model response").toStdString());
    return cleanJSON(text);
}
