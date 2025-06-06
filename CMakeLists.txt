cmake_minimum_required(VERSION 3.15)
project(EQDiff VERSION 1.0.0)

add_subdirectory(JUCE)

# Set VST SDK path - you'll need to download and extract the VST SDK
set(VST_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/VST_SDK" CACHE PATH "Path to VST SDK")
juce_set_vst2_sdk_path(${VST_SDK_PATH})

juce_add_plugin(EQDiff
    VERSION 1.0.0
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Ycom
    PLUGIN_CODE EqDf
    FORMATS VST3 Standalone
    PRODUCT_NAME "EQDiff")

# Set Windows-specific settings
if(WIN32)
    set_target_properties(EQDiff PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
endif()

target_sources(EQDiff
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_features(EQDiff PRIVATE cxx_std_17)

target_include_directories(EQDiff
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules
        ${CMAKE_CURRENT_BINARY_DIR}/EQDiff_artefacts/JuceLibraryCode)

target_link_libraries(EQDiff
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

juce_generate_juce_header(EQDiff)

# Disable VST2 support
set(JUCE_VST3_CAN_REPLACE_VST2 FALSE CACHE BOOL "Disable VST2 support" FORCE) 