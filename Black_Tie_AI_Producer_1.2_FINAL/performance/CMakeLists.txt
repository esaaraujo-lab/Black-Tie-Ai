cmake_minimum_required(VERSION 3.24)
project(BlackTieLive VERSION 1.0.0 LANGUAGES CXX)
include(FetchContent)
set(JUCE_TAG 9.0.1 CACHE STRING "Pinned JUCE version")
set(JUCE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/JUCE" CACHE PATH "Existing JUCE checkout")
if(EXISTS "${JUCE_SOURCE_DIR}/CMakeLists.txt")
  add_subdirectory("${JUCE_SOURCE_DIR}" juce)
else()
  FetchContent_Declare(juce GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG ${JUCE_TAG} GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(juce)
endif()
juce_add_plugin(BlackTieLive
  COMPANY_NAME "Black Tie AI"
  BUNDLE_ID com.blacktie.ai.live
  PLUGIN_MANUFACTURER_CODE BtAi
  PLUGIN_CODE BtLv
  PLUGIN_NAME "Black Tie Live"
  PRODUCT_NAME "Black Tie Live"
  DESCRIPTION "Live-performance MIDI clock, routing and performance control layer"
  FORMATS VST3 AU
  IS_SYNTH FALSE
  IS_MIDI_EFFECT TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT TRUE
  AU_MAIN_TYPE kAudioUnitType_MIDIProcessor
  COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(BlackTieLive PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_features(BlackTieLive PRIVATE cxx_std_17)
target_compile_definitions(BlackTieLive PRIVATE JUCE_WEB_BROWSER=0)
target_link_libraries(BlackTieLive PRIVATE juce::juce_audio_utils juce::juce_audio_basics juce::juce_gui_extra)
