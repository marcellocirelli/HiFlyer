/*
  ==============================================================================

    PhaserProcessor.cpp
    Created: 18 Feb 2026 11:24:31am
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "PhaserProcessor.h"

void PhaserProcessor::prepare(const juce::dsp::ProcessSpec &spec)
{
    upperBank.prepare(spec);
    lowerBank.prepare(spec);
}

void PhaserProcessor::reset()
{
    upperBank.reset();
    lowerBank.reset();
}

void PhaserProcessor::setTreatment(Treatment newTreatment) noexcept
{
    treatment = newTreatment;
    applyTreatment();
}

void PhaserProcessor::setFeedback(float amount) noexcept
{
    feedbackAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void PhaserProcessor::setAudioToModDepth(float depth) noexcept
{
    audioToModDepth = juce::jlimit(0.0f, 1.0f, depth);
}

void PhaserProcessor::applyTreatment() noexcept
{
    switch (treatment)
    {
        case Treatment::Vibrato:
            feedbackEnabled = false;
            audioToMod      = false;
            invertLowerMod  = false;
            lowerStages     = 6;
            lowerOutputTap  = 6;
            mixMode         = MixMode::WetOnly;
            break;
            
        case Treatment::Phasing1:
            feedbackEnabled = false;
            audioToMod      = false;
            invertLowerMod  = false;
            lowerStages     = 6;
            lowerOutputTap  = 6;
            mixMode         = MixMode::DryPlusWet;
            break;
            
        case Treatment::Phasing2:
            feedbackEnabled = false;
            audioToMod      = false;
            invertLowerMod  = false;
            lowerStages     = 6;
            lowerOutputTap  = 6;
            mixMode         = MixMode::DryHighGainPlusWet;
            break;
            
        case Treatment::Waa:
            feedbackEnabled = true;
            audioToMod      = true;
            invertLowerMod  = false;
            lowerStages     = 6;
            lowerOutputTap  = 2;
            mixMode         = MixMode::WetOnly;
            break;
            
        case Treatment::Waw:
            feedbackEnabled = true;
            audioToMod      = true;
            invertLowerMod  = false;
            lowerStages     = 6;
            lowerOutputTap  = 6;
            mixMode         = MixMode::UpperPlusWet;
            break;
            
        case Treatment::Meow:
            feedbackEnabled = true;
            audioToMod      = true;
            invertLowerMod  = true;
            lowerStages     = 6;
            lowerOutputTap  = 6;
            mixMode         = MixMode::UpperPlusWet;
            break;
    }
    
    const float upperFb = feedbackEnabled ? feedbackAmount * kBank1FbAtten : 0.0f;
    const float lowerFb = feedbackEnabled ? feedbackAmount * kBank2FbAtten : 0.0f;
    upperBank.setFeedback(upperFb);
    lowerBank.setFeedback(lowerFb);
    lowerBank.setOutputTap(lowerOutputTap);
}

float PhaserProcessor::processSample(float input, float modulation) noexcept
{
    // LFO + Frequency Shift
    float mod = juce::jlimit(0.0f, 1.0f, modulation);
    
    // Audio to bow strings if Waa/Waw/Meow
    if (audioToMod)
        mod = juce::jlimit(0.0f, 1.0f, mod + input * audioToModDepth);
    
    const float upperMod = mod;
    const float lowerMod = invertLowerMod ? (1.0f - mod) : mod;
    
    // Signal chain
    const float afterUpper = upperBank.processSample(input, upperMod);
    const float afterLower = lowerBank.processSample(afterUpper, lowerMod);
    
    switch (mixMode)
    {
        case MixMode::WetOnly:
            return afterLower;
        case MixMode::DryPlusWet:
            return (input + afterLower) * 0.5f;
        case MixMode::DryHighGainPlusWet:
            return (input - afterLower) * 0.5f;
        case MixMode::UpperPlusWet:
            return (afterUpper + afterLower) * 0.5f;
    }
    
    return afterLower;
}
