#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

EQDiffAudioProcessorEditor::EQDiffAudioProcessorEditor(EQDiffAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set up the look and feel
    lookAndFeel.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    lookAndFeel.setColour(juce::Slider::trackColourId, juce::Colour(0xFF2D2D2D));
    lookAndFeel.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A));
    lookAndFeel.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2D2D2D));
    lookAndFeel.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF4A4A4A));
    setLookAndFeel(&lookAndFeel);

    // Create EQ sliders
    for (int i = 0; i < 10; ++i)
    {
        eqSliders[i] = std::make_unique<juce::Slider>(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
        eqSliders[i]->setRange(-12.0, 12.0, 0.1);
        eqSliders[i]->setValue(0.0);
        eqSliders[i]->setSliderStyle(juce::Slider::LinearVertical);
        eqSliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        eqSliders[i]->addListener(this);
        addAndMakeVisible(eqSliders[i].get());

        eqLabels[i] = std::make_unique<juce::Label>();
        eqLabels[i]->setText(juce::String(audioProcessor.bandFrequencies[i]) + " Hz", juce::dontSendNotification);
        eqLabels[i]->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(eqLabels[i].get());
    }

    // Create master gain slider
    masterGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    masterGainSlider.setRange(-24.0, 24.0, 0.1);
    masterGainSlider.setValue(0.0);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    masterGainSlider.addListener(this);
    addAndMakeVisible(&masterGainSlider);

    masterGainLabel.setText("Master", juce::dontSendNotification);
    masterGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&masterGainLabel);

    // Create bypass button
    bypassButton.setButtonText("Bypass");
    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    addAndMakeVisible(&bypassButton);

    // Create frequency response display
    freqDisplay = std::make_unique<FrequencyResponseDisplay>(audioProcessor);
    addAndMakeVisible(freqDisplay.get());

    // Set window size
    setSize(800, 600);
}

EQDiffAudioProcessorEditor::~EQDiffAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void EQDiffAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1A1A1A));
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("EQDiff", getLocalBounds().removeFromTop(40), juce::Justification::centred, true);
}

void EQDiffAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40); // Space for title

    // Frequency response display
    freqDisplay->setBounds(area.removeFromTop(200));

    // EQ sliders
    auto sliderArea = area.removeFromTop(300);
    int sliderWidth = sliderArea.getWidth() / 10;
    
    for (int i = 0; i < 10; ++i)
    {
        auto sliderBounds = sliderArea.removeFromLeft(sliderWidth);
        eqLabels[i]->setBounds(sliderBounds.removeFromTop(20));
        eqSliders[i]->setBounds(sliderBounds);
    }

    // Bottom controls
    auto bottomArea = area;
    masterGainLabel.setBounds(bottomArea.removeFromLeft(100).removeFromTop(20));
    masterGainSlider.setBounds(bottomArea.removeFromLeft(100));
    bypassButton.setBounds(bottomArea.removeFromLeft(100).reduced(10));
}

void EQDiffAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    for (int i = 0; i < 10; ++i)
    {
        if (slider == eqSliders[i].get())
        {
            audioProcessor.getEqBandParams()[i]->setValueNotifyingHost(slider->getValue());
            break;
        }
    }
    
    if (slider == &masterGainSlider)
    {
        // Handle master gain changes
    }
}

// Frequency Response Display Implementation
EQDiffAudioProcessorEditor::FrequencyResponseDisplay::FrequencyResponseDisplay(EQDiffAudioProcessor& p)
    : processor(p)
{
    startTimerHz(30); // Update 30 times per second
}

void EQDiffAudioProcessorEditor::FrequencyResponseDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF2D2D2D));
    
    // Draw frequency response
    g.setColour(juce::Colours::white);
    g.strokePath(frequencyResponsePath, juce::PathStrokeType(2.0f));
    
    // Draw frequency markers
    g.setFont(12.0f);
    float freqs[] = { 31.0f, 64.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f };
    for (float freq : freqs)
    {
        float x = std::log10(freq / 20.0f) / std::log10(20000.0f / 20.0f) * getWidth();
        g.drawLine(x, 0, x, getHeight(), 1.0f);
        g.drawText(juce::String(freq) + "Hz", x - 20, getHeight() - 20, 40, 20, juce::Justification::centred);
    }
}

void EQDiffAudioProcessorEditor::FrequencyResponseDisplay::timerCallback()
{
    // Update frequency response path based on current EQ settings
    frequencyResponsePath.clear();
    
    // Calculate frequency response
    const int numPoints = 100;
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = 20.0f * std::pow(20000.0f / 20.0f, static_cast<float>(i) / (numPoints - 1));
        float x = std::log10(freq / 20.0f) / std::log10(20000.0f / 20.0f) * static_cast<float>(getWidth());
        
        // Calculate magnitude response based on EQ settings
        float magnitude = 0.0f;
        for (int band = 0; band < 10; ++band)
        {
            float gain = processor.getEqBandParams()[band]->get();
            float bandFreq = processor.bandFrequencies[band];
            float q = 1.0f;
            
            // Simple peak filter response calculation
            float w = 2.0f * M_PI * freq / static_cast<float>(processor.getSampleRate());
            float w0 = 2.0f * M_PI * bandFreq / static_cast<float>(processor.getSampleRate());
            float alpha = std::sin(w0) / (2.0f * q);
            
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * std::cos(w0);
            float a2 = 1.0f - alpha;
            float b0 = 1.0f + alpha * std::pow(10.0f, gain / 20.0f);
            float b1 = a1;
            float b2 = 1.0f - alpha * std::pow(10.0f, gain / 20.0f);
            
            float h = std::sqrt((b0 * b0 + b1 * b1 + b2 * b2 - 2.0f * (b0 * b1 + b1 * b2) * std::cos(w) + 2.0f * b0 * b2 * std::cos(2.0f * w)) /
                             (a0 * a0 + a1 * a1 + a2 * a2 - 2.0f * (a0 * a1 + a1 * a2) * std::cos(w) + 2.0f * a0 * a2 * std::cos(2.0f * w)));
            
            magnitude += 20.0f * std::log10(h);
        }
        
        float y = (1.0f - (magnitude + 24.0f) / 48.0f) * static_cast<float>(getHeight());
        
        if (i == 0)
            frequencyResponsePath.startNewSubPath(x, y);
        else
            frequencyResponsePath.lineTo(x, y);
    }
    
    repaint();
} 