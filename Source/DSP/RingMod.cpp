/*
  ==============================================================================

    RingMod.cpp
    Created: 24 Mar 2026 11:01:59am
    Author:  Marcello Cirelli

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
    // Q2/Q3 differential pair: soft half-wave rectification of negative half,
    // inverted to positive. tanh knee matches BJT differential pair character.
    const float negInput = -input;
    const float softRect = 0.5f * negInput * (1.0f + std::tanh (negInput / kSoftness));

    // A11 OTA: add controlled amount of rectified component.
    // gain 0–2 maps from fader 0–1 so at amount=1 the fundamental fully cancels.
    return 2.0f * amount * softRect;
}
