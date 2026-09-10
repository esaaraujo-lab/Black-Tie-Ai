# Unattended installation reality

The platform scripts are designed for one-command setup. They install missing open-source/build prerequisites where the OS permits silent package installation, clone the pinned JUCE source, configure both targets, build Release artifacts, and collect them under `dist/`.

A script cannot legitimately bypass OS security/consent mechanisms. In particular, macOS Command Line Tools may require Apple's user/system-admin approval. Likewise, Windows enterprise policies can disable `winget`, software installation, or compiler deployment. When this happens the script stops with a precise prerequisite message instead of claiming success.

The resulting VST3 bundles are the only files that belong in the DAW's VST3 directory. The AI Producer itself does not require Ollama, Node.js, Python, or a separate local server at runtime. The optional Ableton Extensions adapter is a host-side integration and is required only for operations that the VST contract cannot perform itself, such as rewriting arbitrary Live Set structure.
