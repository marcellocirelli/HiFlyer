/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HiFlyerAudioProcessor::HiFlyerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), params(apvts)
#endif
{
}

HiFlyerAudioProcessor::~HiFlyerAudioProcessor()
{
}

//==============================================================================
const juce::String HiFlyerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HiFlyerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HiFlyerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HiFlyerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HiFlyerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HiFlyerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HiFlyerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HiFlyerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HiFlyerAudioProcessor::getProgramName (int index)
{
    return {};
}

void HiFlyerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HiFlyerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;
    
    lfo.prepare(spec);
    for (int ch = 0; ch < maxChannels; ++ch)
    {
        phaser[ch].prepare (spec);
        phaser[ch].setFeedback (0.5f);
        phaser[ch].setAudioToModDepth (0.1f);
    }
    
    params.prepareToPlay(sampleRate);
    params.reset();
}

void HiFlyerAudioProcessor::releaseResources()
{
    lfo.reset();
    for (int ch = 0; ch < maxChannels; ++ch)
        phaser[ch].reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HiFlyerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HiFlyerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    params.update();
    
    lfo.setWaveform(static_cast<LFO::Waveform>(params.controlMod));
    const auto treatment = static_cast<PhaserProcessor::Treatment>(params.treatment);
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        phaser[ch].setTreatment (treatment);
    
    const int numSamples = buffer.getNumSamples();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        params.smoothen();

        lfo.setSpeed (params.modSpeed);
        lfo.setDepth (params.modDepth);

//        const float modulation = juce::jlimit (0.0f, 1.0f, lfo.processSample() + (1.0f - params.freqShift));
        const float lfoValue = lfo.processSample();
        const float center = params.freqShift;
        const float modulation = juce::jlimit (0.0f, 1.0f, center + (lfoValue - params.modDepth * 0.5f));

        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer (ch);

            const float dry = channelData[sample];
            const float wet = phaser[ch].processSample (dry, modulation);
            channelData[sample] = (wet + (dry - wet) * params.mix) * params.gain;
        }
    }
}

//==============================================================================
bool HiFlyerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HiFlyerAudioProcessor::createEditor()
{
    return new HiFlyerAudioProcessorEditor (*this);
}

//==============================================================================
void HiFlyerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void HiFlyerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement>xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HiFlyerAudioProcessor();
}
