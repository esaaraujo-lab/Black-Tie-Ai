# User setup — minimum friction

## Cloud

Enter the API key in the plugin's settings in the final UI build. Keys should be stored using the platform's secure credential store in the production implementation, not as plain text in the project.

## Local Ollama

If Ollama is already installed, select Ollama and set the model. No cloud key is needed.

## Live

For the lowest-friction Live integration, use the official Extensions route on a supported Live build. Install the packaged `.ablx` once in Live Settings → Extensions. The VST then talks to localhost and remains the single producer UI.

## Fallback

If Extensions are unavailable, use the existing Max for Live/Remote Script bridge included in earlier project versions.
