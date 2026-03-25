/*
  ==============================================================================

    SustainFuzz.cpp
    Created: 24 Mar 2026 1:04:24pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "SustainFuzz.h"
#include <cmath>
#include <algorithm>

void SustainFuzz::prepare(const juce::dsp::ProcessSpec &spec)
{
    sampleRate = spec.sampleRate;
    // One-pole LPF coefficient
    const float w = juce::MathConstants<float>::twoPi * kLpfHz / static_cast<float>(sampleRate);
    lpfCoeff = w / (1.0f + w);
    
    reset();
}

void SustainFuzz::reset()
{
    lpfState = 0.0f;
}

float SustainFuzz::processSample(float input, float attackEnvelope, float fuzzLevel) noexcept
{
    const float ceiling = std::max (attackEnvelope, 1e-10f);
    const float bias = ceiling * 0.5f;
    
    // Invert/amplify/offset
    const float amplified = bias + (-input * kGain);
    
    // Clipping
    const float clipped = std::clamp(amplified, 0.0f, ceiling);
    
    // Remove DC offset
    const float acCoupled = clipped - bias;
    
    // Output LPF
    lpfState += lpfCoeff * (acCoupled - lpfState);
    
    return lpfState * kOtaGain * fuzzLevel;
}
