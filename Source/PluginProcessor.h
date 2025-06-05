#pragma once

#include <JuceHeader.h>

class EQDiffAudioProcessor : public juce::AudioProcessor
{
public:
    EQDiffAudioProcessor();
    ~EQDiffAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    const std::array<float, 10> bandFrequencies = {
        20000.0f, 10000.0f, 5000.0f, 2000.0f, 1000.0f,
        500.0f, 200.0f, 100.0f, 50.0f, 20.0f
    };

    const std::array<juce::AudioParameterFloat*, 10>& getEqBandParams() const { return eqBandParams; }

private:
    juce::AudioProcessorValueTreeState parameters;
    std::array<juce::AudioParameterFloat*, 10> eqBandParams;
    std::array<juce::dsp::IIR::Filter<float>, 10> eqFiltersA;
    std::array<juce::dsp::IIR::Filter<float>, 10> eqFiltersB;
    juce::dsp::ProcessSpec processSpec;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQDiffAudioProcessor)
}; 