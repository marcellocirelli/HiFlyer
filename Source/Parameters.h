/*
  ==============================================================================

    Parameters.h
    Created: 6 Feb 2026 2:40:58pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// Parameter IDs
const juce::ParameterID gainParamID {"gain", 1};
const juce::ParameterID fallTimeParamID {"fall time", 1};
const juce::ParameterID riseTimeParamID {"rise time", 1};
const juce::ParameterID controlModParamID {"control mod", 1};
const juce::ParameterID treatmentParamID {"treatment", 1};

const juce::ParameterID topBoostParamID {"top boost", 1};
const juce::ParameterID subOctaveParamID {"sub octave", 1};
const juce::ParameterID ringModParamID {"ring mod", 1};
const juce::ParameterID fuzzLevelParamID {"fuzz level", 1};
const juce::ParameterID mixParamID {"mix", 1};
const juce::ParameterID modSpeedParamID {"mod speed", 1};
const juce::ParameterID modRampParamID {"mod ramp", 1};
const juce::ParameterID modDepthParamID {"mod depth", 1};
const juce::ParameterID freqShiftParamID {"freq shift", 1};

const juce::ParameterID lpTopBoostParamID {"lp top boost", 1};
const juce::ParameterID lpSubOctaveParamID {"lp sub octave", 1};
const juce::ParameterID lpRingModParamID {"lp ring mod", 1};
const juce::ParameterID lpFuzzLevelParamID {"lp fuzz level", 1};
const juce::ParameterID rpModSpeedParamID {"rp mod speed", 1};
const juce::ParameterID rpModRampParamID {"rp mod ramp", 1};
const juce::ParameterID rpModDepthParamID {"rp mod depth", 1};
const juce::ParameterID rpFreqShiftParamID {"rp freq shift", 1};

const juce::ParameterID triggerSensParamID {"trigger sens", 1};
const juce::ParameterID buzzParamID {"buzz", 1};
const juce::ParameterID growlParamID {"growl", 1};

class Parameters
{
    public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void update() noexcept;
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    
    float gain = 0.0f;
    float fallTime = 100.0f;
    float riseTime = 0.0f;
    float topBoost = 0.0f;
    float subOctave = 0.0f;
    float ringMod = 0.0f;
    float fuzzLevel = 0.0f;
    float mix = 0.0f;
    float modSpeed = 0.0f;
    float modRamp = 0.0f;
    float modDepth = 0.0f;
    float freqShift = 0.0f;
    bool triggerSens = 1;
    bool buzz = 0;
    int growl = 1;
    int controlMod = 1;
    int treatment = 0;
    int lpTopBoost = 1;
    int lpSubOctave = 1;
    int lpRingMod = 1;
    int lpFuzzLevel = 1;
    int rpModSpeed = 1;
    int rpModRamp = 1;
    int rpModDepth = 1;
    int rpFreqShift = 1;
    
    private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    
    juce::AudioParameterFloat* fallTimeParam;
    juce::LinearSmoothedValue<float> fallTimeSmoother;
    
    juce::AudioParameterFloat* riseTimeParam;
    juce::LinearSmoothedValue<float> riseTimeSmoother;
    
    juce::AudioParameterFloat* topBoostParam;
    juce::LinearSmoothedValue<float> topBoostSmoother;

    juce::AudioParameterFloat* subOctaveParam;
    juce::LinearSmoothedValue<float> subOctaveSmoother;
    
    juce::AudioParameterFloat* ringModParam;
    juce::LinearSmoothedValue<float> ringModSmoother;
    
    juce::AudioParameterFloat* fuzzLevelParam;
    juce::LinearSmoothedValue<float> fuzzLevelSmoother;
    
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    
    juce::AudioParameterFloat* modSpeedParam;
    juce::LinearSmoothedValue<float> modSpeedSmoother;
    
    juce::AudioParameterFloat* modRampParam;
    juce::LinearSmoothedValue<float> modRampSmoother;
    
    juce::AudioParameterFloat* modDepthParam;
    juce::LinearSmoothedValue<float> modDepthSmoother;
    
    juce::AudioParameterFloat* freqShiftParam;
    juce::LinearSmoothedValue<float> freqShiftSmoother;
    
    juce::AudioParameterBool* buzzParam;
    juce::AudioParameterChoice* growlParam;
    juce::AudioParameterChoice* triggerSensParam;
    juce::AudioParameterChoice* controlModParam;
    juce::AudioParameterChoice* treatmentParam;
    juce::AudioParameterChoice* lpTopBoostParam;
    juce::AudioParameterChoice* lpSubOctaveParam;
    juce::AudioParameterChoice* lpRingModParam;
    juce::AudioParameterChoice* lpFuzzLevelParam;
    juce::AudioParameterChoice* rpModSpeedParam;
    juce::AudioParameterChoice* rpModRampParam;
    juce::AudioParameterChoice* rpModDepthParam;
    juce::AudioParameterChoice* rpFreqShiftParam;

};
