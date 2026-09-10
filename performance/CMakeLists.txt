cmake_minimum_required(VERSION 3.24)
project(BlackTieLive VERSION 1.0.0 LANGUAGES C CXX)
if(MSVC)
    add_compile_options(/utf-8)
endif()

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

juce_add_plugin(BlackTieLive
    COMPANY_NAME "ChatGPT Ableton Control"
    BUNDLE_ID com.gptableton.blacktielive
    PLUGIN_MANUFACTURER_CODE Gtac
    PLUGIN_CODE BtLv
    FORMATS VST3 AU
    PRODUCT_NAME "Black Tie Live"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(BlackTieLive)

if(TARGET juce_audio_processors)
    target_compile_definitions(juce_audio_processors INTERFACE
        JUCE_PLUGINHOST_VST3=1 JUCE_PLUGINHOST_AU=1)
endif()
if(TARGET juce_audio_processors_headless)
    target_compile_definitions(juce_audio_processors_headless INTERFACE
        JUCE_PLUGINHOST_VST3=1 JUCE_PLUGINHOST_AU=1)
endif()

file(GLOB BLACKTIE_LIVE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
file(GLOB AGENT_CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../agent_core/*.cpp)

target_sources(BlackTieLive PRIVATE
    ${BLACKTIE_LIVE_SOURCES}
    ${AGENT_CORE_SOURCES})

target_include_directories(BlackTieLive PRIVATE ../agent_core)
target_compile_features(BlackTieLive PRIVATE cxx_std_17)
target_compile_definitions(BlackTieLive PRIVATE
    JUCE_PLUGINHOST_VST3=1
    JUCE_PLUGINHOST_AU=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(BlackTieLive PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_cryptography)