/*
  ==============================================================================

    Parameters.cpp
    Created: 6 Feb 2026 2:40:58pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "Parameters.h"

juce::StringArray modulationTypes = {
    "Sine Slow",
    "Sine Fast",
    "Sine Ramp Up",
    "Sine Ramp Down",
    "Ramp Up",
    "Ramp Down"
};

juce::StringArray treatmentTypes = {
    "Vibrato",
    "Phasing 1",
    "Phasing 2",
    "Waa",
    "Waw",
    "Meow"
};

juce::StringArray pedalPos = {
    "Reverse",
    "Off",
    "Normal"
};

juce::StringArray triggerSensPos = {
    "Strum",
    "Solo"
};

juce::StringArray growlPos = {
    "2",
    "Off",
    "4"
};

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

static juce::String stringFromPercent(float value, int)
{
    return juce::String(int(value)) + " %";
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, fallTimeParamID, fallTimeParam);
    castParameter(apvts, riseTimeParamID, riseTimeParam);
    castParameter(apvts, controlModParamID, controlModParam);
    castParameter(apvts, treatmentParamID, treatmentParam);
    castParameter(apvts, topBoostParamID, topBoostParam);
    castParameter(apvts, subOctaveParamID, subOctaveParam);
    castParameter(apvts, ringModParamID, ringModParam);
    castParameter(apvts, fuzzLevelParamID, fuzzLevelParam);
    castParameter(apvts, mixParamID, mixParam);
    castParameter(apvts, modSpeedParamID, modSpeedParam);
    castParameter(apvts, modRampParamID, modRampParam);
    castParameter(apvts, modDepthParamID, modDepthParam);
    castParameter(apvts, freqShiftParamID, freqShiftParam);
    castParameter(apvts, lpTopBoostParamID, lpTopBoostParam);
    castParameter(apvts, lpSubOctaveParamID, lpSubOctaveParam);
    castParameter(apvts, lpRingModParamID, lpRingModParam);
    castParameter(apvts, lpFuzzLevelParamID, lpFuzzLevelParam);
    castParameter(apvts, rpModSpeedParamID, rpModSpeedParam);
    castParameter(apvts, rpModRampParamID, rpModRampParam);
    castParameter(apvts, rpModDepthParamID, rpModDepthParam);
    castParameter(apvts, rpFreqShiftParamID, rpFreqShiftParam);
    castParameter(apvts, triggerSensParamID, triggerSensParam);
    castParameter(apvts, buzzParamID, buzzParam);
    castParameter(apvts, growlParamID, growlParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        gainParamID,
        "Output",
        juce::NormalisableRange<float> { -24.0f, 6.0f},
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        fallTimeParamID,
        "Fall Time",
        juce::NormalisableRange<float> { 0.0f, 100.0f},
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        riseTimeParamID,
        "Rise Time",
        juce::NormalisableRange<float> { 0.0f, 100.0f},
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        controlModParamID,
        "Control Modulation",
        modulationTypes,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        treatmentParamID,
        "Treatment",
        treatmentTypes,
        0));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        topBoostParamID,
        "Top Boost",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        subOctaveParamID,
        "Sub Octave",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ringModParamID,
        "Ring Modulation",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        fuzzLevelParamID,
        "Fuzz Level",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        mixParamID,
        "Bypass Mix",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        modSpeedParamID,
        "Modulation Speed",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        50.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        modRampParamID,
        "Modulation Ramp Time",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        50.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        modDepthParamID,
        "Modulation Depth",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        freqShiftParamID,
        "Frequency Shift",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
        30.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        lpTopBoostParamID,
        "LP Top Boost",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        lpSubOctaveParamID,
        "LP Sub Octave",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        lpRingModParamID,
        "LP Ring Modulation",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        lpFuzzLevelParamID,
        "LP Fuzz Level",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        rpModSpeedParamID,
        "RP Modulation Speed",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        rpModRampParamID,
        "RP Modulation Ramp Time",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        rpModDepthParamID,
        "RP Modulation Depth",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        rpFreqShiftParamID,
        "RP Frequency Shift",
        pedalPos,
        1));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        triggerSensParamID,
        "Trigger Sensitivity",
        triggerSensPos,
        true));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(
        buzzParamID,
        "Buzz",
        false));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        growlParamID,
        "Growl",
        growlPos,
        1));
    
    return layout;
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    fallTimeSmoother.setTargetValue(fallTimeParam->get() * 0.01f);
    riseTimeSmoother.setTargetValue(riseTimeParam->get() * 0.01f);
    topBoostSmoother.setTargetValue(topBoostParam->get() * 0.01f);
    subOctaveSmoother.setTargetValue(subOctaveParam->get() * 0.01f);
    ringModSmoother.setTargetValue(ringModParam->get() * 0.01f);
    fuzzLevelSmoother.setTargetValue(fuzzLevelParam->get() * 0.01f);
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);
    modSpeedSmoother.setTargetValue(modSpeedParam->get() * 0.01f);
    modRampSmoother.setTargetValue(modRampParam->get() * 0.01f);
    modDepthSmoother.setTargetValue(modDepthParam->get() * 0.01f);
    freqShiftSmoother.setTargetValue(freqShiftParam->get() * 0.01f);
    
    lpTopBoost = lpTopBoostParam->getIndex();
    lpSubOctave = lpSubOctaveParam->getIndex();
    lpRingMod = lpRingModParam->getIndex();
    lpFuzzLevel = lpFuzzLevelParam->getIndex();
    rpModSpeed = rpModSpeedParam->getIndex();
    rpModRamp = rpModRampParam->getIndex();
    rpModDepth = rpModDepthParam->getIndex();
    rpFreqShift = rpFreqShiftParam->getIndex();
    triggerSens = triggerSensParam->getIndex();
    buzz = buzzParam->get();
    growl = growlParam->getIndex();
    controlMod = controlModParam->getIndex();
    treatment = treatmentParam->getIndex();
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);
    fallTimeSmoother.reset(sampleRate, duration);
    riseTimeSmoother.reset(sampleRate, duration);
    topBoostSmoother.reset(sampleRate, duration);
    subOctaveSmoother.reset(sampleRate, duration);
    ringModSmoother.reset(sampleRate, duration);
    fuzzLevelSmoother.reset(sampleRate, duration);
    mixSmoother.reset(sampleRate, duration);
    modSpeedSmoother.reset(sampleRate, duration);
    modRampSmoother.reset(sampleRate, duration);
    modDepthSmoother.reset(sampleRate, duration);
    freqShiftSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
    gain = 0.0f;
    fallTime = 0.0f;
    riseTime = 0.0f;
    topBoost = 0.0f;
    subOctave = 0.0f;
    ringMod = 0.0f;
    fuzzLevel = 0.0f;
    mix = 1.0f;
    modSpeed = 0.0f;
    modRamp = 0.0f;
    modDepth = 0.0f;
    freqShift = 0.0f;
    triggerSens = 0;
    buzz = 0;
    growl = 0;
    controlMod = 0;
    treatment = 0;
    lpTopBoost = 1;
    lpSubOctave = 1;
    lpRingMod = 1;
    lpFuzzLevel = 1;
    rpModSpeed = 1;
    rpModRamp = 1;
    rpModDepth = 1;
    rpFreqShift = 1;
    
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    fallTimeSmoother.setCurrentAndTargetValue(fallTimeParam->get());
    riseTimeSmoother.setCurrentAndTargetValue(riseTimeParam->get());
    topBoostSmoother.setCurrentAndTargetValue(topBoostParam->get());
    subOctaveSmoother.setCurrentAndTargetValue(subOctaveParam->get());
    ringModSmoother.setCurrentAndTargetValue(ringModParam->get());
    fuzzLevelSmoother.setCurrentAndTargetValue(fuzzLevelParam->get());
    mixSmoother.setCurrentAndTargetValue(mixParam->get());
    modSpeedSmoother.setCurrentAndTargetValue(modSpeedParam->get());
    modRampSmoother.setCurrentAndTargetValue(modRampParam->get());
    modDepthSmoother.setCurrentAndTargetValue(modDepthParam->get());
    freqShiftSmoother.setCurrentAndTargetValue(freqShiftParam->get());
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    fallTime = fallTimeSmoother.getNextValue();
    riseTime = riseTimeSmoother.getNextValue();
    topBoost = topBoostSmoother.getNextValue();
    subOctave = subOctaveSmoother.getNextValue();
    ringMod = ringModSmoother.getNextValue();
    fuzzLevel = fuzzLevelSmoother.getNextValue();
    mix = mixSmoother.getNextValue();
    modSpeed = modSpeedSmoother.getNextValue();
    modRamp = modRampSmoother.getNextValue();
    modDepth = modDepthSmoother.getNextValue();
    freqShift = freqShiftSmoother.getNextValue();
}
