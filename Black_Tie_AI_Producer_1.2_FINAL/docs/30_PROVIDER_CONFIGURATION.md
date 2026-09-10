# Provider configuration — FINAL 1.1

## OpenAI

Set:

- Enabled: yes/no
- Base URL: normally `https://api.openai.com/v1`
- Model: editable; use a model available to your account
- API key: your OpenAI key

The plugin sends an OpenAI-compatible Chat Completions request.

## Google Gemini

Set the Gemini OpenAI-compatible base URL and a current model supported by your account. The exact current model should remain editable because Google changes model availability over time.

## NVIDIA NIM

Set the NIM OpenAI-compatible base URL, model identifier and API key. Model identifiers are editable and should match the NIM deployment/account.

## Ollama Local

Default:

`http://127.0.0.1:11434/v1`

No Ollama installation is required for OpenAI/Gemini/NVIDIA operation. Ollama is an optional provider.

## Ollama Cloud

Set the Ollama Cloud OpenAI-compatible endpoint and API key. It is independent of the local endpoint. In single-provider Ollama mode, local is preferred when enabled; cloud is used when local is not enabled.

## One provider vs all

**1 API** is the recommended default for predictable cost and latency.

**All APIs** is an ensemble/review mode. The first successful provider generates the candidate JSON; the next enabled providers receive that candidate and are asked to validate/correct it. This can multiply API usage and latency.

## Secrets

API keys are held in memory by default. Enabling project-state saving writes them into the plugin state. Do not use that option for projects that will be shared or uploaded to untrusted locations.
