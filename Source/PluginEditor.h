#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

class EQDiffAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 public juce::Slider::Listener
{
public:
    EQDiffAudioProcessorEditor(EQDiffAudioProcessor&);
    ~EQDiffAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    EQDiffAudioProcessor& audioProcessor;
    
    // Custom look and feel
    juce::LookAndFeel_V4 lookAndFeel;
    
    // EQ Band sliders
    std::array<std::unique_ptr<juce::Slider>, 10> eqSliders;
    std::array<std::unique_ptr<juce::Label>, 10> eqLabels;
    
    // Master controls
    juce::Slider masterGainSlider;
    juce::Label masterGainLabel;
    
    // Bypass button
    juce::TextButton bypassButton;
    
    // Frequency response display
    class FrequencyResponseDisplay : public juce::Component,
                                    public juce::Timer
    {
    public:
        FrequencyResponseDisplay(EQDiffAudioProcessor& p);
        void paint(juce::Graphics& g) override;
        void timerCallback() override;
        
    private:
        EQDiffAudioProcessor& processor;
        juce::Path frequencyResponsePath;
    };
    
    std::unique_ptr<FrequencyResponseDisplay> freqDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQDiffAudioProcessorEditor)
}; 