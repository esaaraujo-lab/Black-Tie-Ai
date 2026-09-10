cmake_minimum_required(VERSION 3.24)
project(ChatGPTAbletonAIProducer VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
set(JUCE_TAG 9.0.1 CACHE STRING "Pinned JUCE version")
set(JUCE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/JUCE" CACHE PATH "Existing JUCE checkout")

if(EXISTS "${JUCE_SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_SOURCE_DIR}" juce)
else()
    FetchContent_Declare(juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG ${JUCE_TAG}
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(juce)
endif()

juce_add_plugin(ChatGPTAbletonAIProducer
    COMPANY_NAME "ChatGPT Ableton Control"
    BUNDLE_ID com.gptableton.aiproducer
    PLUGIN_MANUFACTURER_CODE Gtac
    PLUGIN_CODE Gpa1
    FORMATS VST3 AU
    PRODUCT_NAME "ChatGPT × Ableton AI Producer"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGINHOST_VST3 TRUE
    PLUGINHOST_AU TRUE)

target_sources(ChatGPTAbletonAIProducer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    ../agent_core/AgentHttp.cpp
    ../agent_core/ProviderClient.cpp
    ../agent_core/ProviderRouter.cpp
    ../agent_core/ProducerAgent.cpp
    ../agent_core/SampleIndex.cpp
    ../agent_core/PluginHost.cpp
    ../agent_core/LocalAudioEngine.cpp)

target_include_directories(ChatGPTAbletonAIProducer PRIVATE ../agent_core)
target_compile_features(ChatGPTAbletonAIProducer PRIVATE cxx_std_17)
target_compile_definitions(ChatGPTAbletonAIProducer PRIVATE
        JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)
target_link_libraries(ChatGPTAbletonAIProducer PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_cryptography)
