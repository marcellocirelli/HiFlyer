/*
  ==============================================================================

    Growl.cpp
    Created: 24 Mar 2026 8:22:49pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "Growl.h"
#include <cmath>

namespace
{
constexpr float kEdgeThreshold = 0.0005f;
}

void Growl::prepare (const juce::dsp::ProcessSpec& spec)
{
    juce::ignoreUnused (spec);
    reset();
}

void Growl::reset()
{
    prevSample = 0.0f;
    ff1 = false;
    prevFF1 = false;
    ff2 = false;
}

float Growl::processSample (float subOctaveInput, int mode) noexcept
{
    // Existing mode convention preserved from PluginProcessor:
    // mode == 1: growl off
    // mode == 0: divide incoming sub octave by 2  -> one more octave down
    // mode == 2: divide incoming sub octave by 4  -> two more octaves down
    if (mode == 1)
    {
        prevSample = subOctaveInput;
        return 0.0f;
    }

    // The input is expected to be the SubOctave module's divided square/slewed
    // square. Use a thresholded rising-edge detector instead of a raw zero-cross
    // so small residual filter movement does not falsely clock the growl divider.
    const float amplitude = std::abs (subOctaveInput);

    const bool crossed = (prevSample <= kEdgeThreshold && subOctaveInput > kEdgeThreshold);
    prevSample = subOctaveInput;

    // First growl flip-flop: divide the sub octave by 2.
    if (crossed)
        ff1 = ! ff1;

    // Second growl flip-flop: divide the already-divided growl by another 2.
    const bool ff1Rise = (ff1 && ! prevFF1);
    if (mode == 2 && ff1Rise)
        ff2 = ! ff2;

    prevFF1 = ff1;

    const bool state = (mode == 0) ? ff1 : ff2;
    return (state ? 1.0f : -1.0f) * amplitude;
}
