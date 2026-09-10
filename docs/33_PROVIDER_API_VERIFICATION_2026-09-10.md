# Provider API verification — 2026-09-10

## OpenAI

The official OpenAI model catalogue currently lists GPT-5.6 Luna with model ID `gpt-5.6-luna`; it supports Chat Completions. The plugin default is therefore `gpt-5.6-luna`, but the model field is editable so users can select any model available to their account.

Official reference: https://developers.openai.com/api/docs/models/gpt-5.6-luna

## Gemini

Google's current OpenAI-compatibility documentation shows the base URL `https://generativelanguage.googleapis.com/v1beta/openai/` and `chat/completions`. The current example on the official page uses `gemini-3.8-flash`; the plugin default was updated accordingly. Google explicitly notes that the compatibility layer has feature limitations, so a future provider implementation may use Gemini's native REST API when advanced Gemini-only features are needed.

Official reference: https://ai.google.dev/gemini-api/docs/openai

## NVIDIA NIM

NVIDIA's current NIM documentation specifies `POST /v1/chat/completions` as an OpenAI-compatible inference endpoint and `GET /v1/models` for available models. The plugin therefore treats the NIM base URL and model ID as editable configuration rather than assuming one permanent model.

Official reference: https://docs.nvidia.com/nim/large-language-models/latest/api-reference.html

## Ollama

Ollama is optional. Its official OpenAI compatibility documentation shows `http://localhost:11434/v1/` and `/v1/chat/completions` for local use. Ollama Cloud can also be reached through `https://ollama.com/v1/` with an API key according to Ollama's integration documentation.

Official references:
- https://docs.ollama.com/api/openai-compatibility
- https://docs.ollama.com/integrations/droid

## Product decision

The VST never launches or requires Ollama for cloud providers. The four provider families are independent. Ollama local/cloud are optional entries in the same provider router. Users can enable only OpenAI, only Gemini, only NVIDIA, only Ollama, or all enabled endpoints.
