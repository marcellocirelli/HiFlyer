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
                       ), params (apvts)
#endif
{
}

HiFlyerAudioProcessor::~HiFlyerAudioProcessor() = default;

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
    return 1;
}

int HiFlyerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HiFlyerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String HiFlyerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void HiFlyerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void HiFlyerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 1;
    
    lfo.prepare (spec);
    for (int ch = 0; ch < maxChannels; ++ch)
    {
        phaser[ch].prepare (spec);
        phaser[ch].setFeedback (0.5f);
        phaser[ch].setAudioToModDepth (0.1f);
        topBoost[ch].prepare (spec);
        envelopeDetector[ch].prepare (spec);
        subOct[ch].prepare (spec);
        ringMod[ch].prepare (spec);
        sustainFuzz[ch].prepare (spec);
        growl[ch].prepare (spec);
    }
    
    params.prepareToPlay (sampleRate);
    params.reset();
}

void HiFlyerAudioProcessor::releaseResources()
{
    lfo.reset();
    for (int ch = 0; ch < maxChannels; ++ch)
    {
        phaser[ch].reset();
        topBoost[ch].reset();
        envelopeDetector[ch].reset();
        subOct[ch].reset();
        ringMod[ch].reset();
        sustainFuzz[ch].reset();
        growl[ch].reset();
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HiFlyerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

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
    juce::ignoreUnused (midiMessages);

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    params.update();

    lfo.setWaveform (static_cast<LFO::Waveform> (params.controlMod));

    const auto treatment = static_cast<PhaserProcessor::Treatment> (params.treatment);
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        phaser[ch].setTreatment (treatment);

    const bool soloMode = (params.triggerSens == 1);
    const bool isSineMode = (params.controlMod <= 3);
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        params.smoothen();

        lfo.setSpeed (params.modSpeed);
        lfo.setDepth (params.modDepth);

        bool blockAttackTrigger = false;
        bool blockDecayTrigger = false;
        EnvelopeOutput envByChannel[maxChannels];

        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            const auto* channelData = buffer.getReadPointer (ch);
            const float dry = channelData[sample];

            // The fall envelope is applied immediately after detection and before
            // every audio effect. The detector itself listens to the raw input so
            // the fall control cannot suppress its own retriggering
            envByChannel[ch] = envelopeDetector[ch].processSample (dry, dry,
                                                                   params.fallTime,
                                                                   params.riseTime,
                                                                   soloMode);

            if (envByChannel[ch].attackTrigger)
                blockAttackTrigger = true;
            if (envByChannel[ch].decayTrigger)
                blockDecayTrigger = true;
        }

        const float lfoValue = lfo.processSample (blockAttackTrigger,
                                                  blockDecayTrigger,
                                                  params.modRamp);

        const float center = params.freqShift;
        const float modulation = juce::jlimit (0.0f, 1.0f,
                                               center + (lfoValue - params.modDepth * 0.5f));

        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer (ch);

            const float dry = channelData[sample];
            const auto& env = envByChannel[ch];

            // Signal order: Fall rate > top boost > sub octave / ring mod > fuzz > phaser
            const float fallen = dry * env.decayEnvelope;

            const float boosted = topBoost[ch].processSample (fallen, params.topBoost);

            const float audibleSub = subOct[ch].processSample (boosted, !params.buzz);
            const float subCarrier = subOct[ch].getCarrier();
            const float growlOut = growl[ch].processSample (subCarrier, params.growl);

            const float ringComponent = ringMod[ch].processSample (boosted, params.ringMod);

            const float effectedPre = boosted + audibleSub * params.subOctave + ringComponent;
            const float fuzzWet = sustainFuzz[ch].processSample (effectedPre, env.attackEnvelope);
            const float effected = effectedPre + fuzzWet * params.fuzzLevel;

            float chanModulation = modulation;
            if (isSineMode && params.growl != 1)
                chanModulation = juce::jlimit (0.0f, 1.0f, modulation + growlOut * params.modDepth);

            const float wet = phaser[ch].processSample (effected, chanModulation);

            channelData[sample] = (dry + (wet - dry) * params.mix) * params.gain;
            
        }
    }
}

//==============================================================================
bool HiFlyerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* HiFlyerAudioProcessor::createEditor()
{
    return new HiFlyerAudioProcessorEditor (*this);
}

//==============================================================================
void HiFlyerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary (*apvts.copyState().createXml(), destData);
}

void HiFlyerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// This creates new instances of the plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HiFlyerAudioProcessor();
}
