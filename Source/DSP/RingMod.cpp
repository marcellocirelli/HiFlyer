/*
  ==============================================================================

    RingMod.cpp
    Created: 24 Mar 2026 11:01:59am
    Author:  Marcello Cirelli

    The ring modulator is not a traditional ring mod. This simply takes the input
    and produces a half-wave rectified version of it. The parameter on the original
    hardware is designed to fully cancel the fundamental at its maximum position.
    I added a soft knee to emulate the imperfections of the original hardware.
 
  ==============================================================================
*/

#include "RingMod.h"
#include <cmath>

void RingMod::prepare (const juce::dsp::ProcessSpec& spec)
{
    juce::ignoreUnused (spec);
    reset();
}

void RingMod::reset()
{
}

float RingMod::processSample (float input, float amount) noexcept
{
    const float negInput = -input;
    const float softRect = 0.5f * negInput * (1.0f + std::tanh (negInput / kSoftness));

    return 2.0f * amount * softRect;
}
