/*
  ==============================================================================

    RingMod.cpp
    Created: 24 Mar 2026 11:01:59am
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "RingMod.h"
#include <cmath>

void RingMod::prepare(const juce::dsp::ProcessSpec &spec)
{
    reset();
}

void RingMod::reset()
{
    
}

float RingMod::processSample(float input, float amount) noexcept
{
    const float negInput = -input;
    const float softRect = 0.5f * negInput * (1.0f + std::tanh(negInput / kSoftness));
    
    return input + 2.0f * amount * softRect;
}
