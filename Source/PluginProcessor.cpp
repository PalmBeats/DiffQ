#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

EQDiffAudioProcessor::EQDiffAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int i = 0; i < 10; ++i)
    {
        eqBandParams[i] = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("band" + juce::String(i)));
    }
}

EQDiffAudioProcessor::~EQDiffAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout EQDiffAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    std::array<float, 10> freqs = {
        20000.0f, 10000.0f, 5000.0f, 2000.0f, 1000.0f,
        500.0f, 200.0f, 100.0f, 50.0f, 20.0f
    };
    for (int i = 0; i < 10; ++i)
    {
        juce::String paramId = "band" + juce::String(i);
        juce::String paramName = juce::String(freqs[i], 0) + " Hz";
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(paramId, 1),
            paramName,
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1) + " dB"; }));
    }
    return { params.begin(), params.end() };
}

void EQDiffAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = getTotalNumOutputChannels();

    for (int i = 0; i < 10; ++i)
    {
        float gain = eqBandParams[i]->get();
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate,
            bandFrequencies[i],
            0.7f,
            std::pow(10.0f, gain / 20.0f));
        eqFiltersA[i].coefficients = coeffs;
        eqFiltersA[i].prepare(processSpec);
        eqFiltersB[i].coefficients = coeffs;
        eqFiltersB[i].prepare(processSpec);
    }
}

void EQDiffAudioProcessor::releaseResources()
{
}

void EQDiffAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't have input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Create a copy of the input for the EQ chain
    juce::AudioBuffer<float> eqBuffer;
    eqBuffer.makeCopyOf(buffer);

    // Process each band in the EQ chain
    for (int i = 0; i < 10; ++i)
    {
        float gain = eqBandParams[i]->get();
        float currentGain = std::pow(10.0f, gain / 20.0f);
        
        // Create a narrow bandpass filter for this frequency
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            getSampleRate(),
            bandFrequencies[i],
            2.0f,  // More moderate Q for better frequency overlap and less low-end loss
            currentGain);
            
        // Process this band in both EQ chains
        eqFiltersA[i].coefficients = coeffs;
        eqFiltersB[i].coefficients = coeffs;
        
        // Process each channel separately
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            juce::dsp::AudioBlock<float> channelBlock(eqBuffer.getArrayOfWritePointers() + channel, 1, buffer.getNumSamples());
            juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);
            
            if (channel == 0)
                eqFiltersA[i].process(channelContext);
            else
                eqFiltersB[i].process(channelContext);
        }
    }

    // Invert the EQ chain
    eqBuffer.applyGain(-1.0f);

    // Mix the original signal with the inverted EQ chain
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float original = buffer.getSample(channel, sample);
            float eq = eqBuffer.getSample(channel, sample);
            buffer.setSample(channel, sample, original + eq);
        }
    }
}

juce::AudioProcessorEditor* EQDiffAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void EQDiffAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void EQDiffAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQDiffAudioProcessor();
} 