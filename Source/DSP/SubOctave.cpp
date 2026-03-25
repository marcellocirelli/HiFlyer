/*
  ==============================================================================

    SubOctave.cpp
    Created: 23 Mar 2026 4:07:20pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "SubOctave.h"
#include <algorithm>

static float msToCoeff (double sampleRate, float ms) noexcept
{
    return 1.0f - std::exp (-1.0f / (static_cast<float> (sampleRate) * ms * 0.001f));
}
 
void SubOctave::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate     = spec.sampleRate;
    preFilterCoeff = msToCoeff (sampleRate, kPreFilterMs);
    envCoeff       = msToCoeff (sampleRate, kEnvTimeMs);
    buzzCoeff      = msToCoeff (sampleRate, kBuzzCutoffMs);
    reset();
}
 
void SubOctave::reset()
{
    preFilter = 0.0f;
    posEnv = 0.0f;
    negEnv = 0.0f;
    ff1 = false;
    prevFF1 = false;
    ff2 = false;
    buzzFilter = 0.0f;
}
 
float SubOctave::processSample (float input, bool buzz) noexcept
{
    // Pre-filter
    preFilter += preFilterCoeff * (input - preFilter);
 
    // Envelope followers
    const float posRect = std::max (0.0f, preFilter);
    const float negRect = std::min (0.0f, preFilter);
 
    // Positive peak detector
    if (posRect > posEnv)
        posEnv = posRect;
    else
        posEnv -= envCoeff * posEnv;
 
    // Negative peak detector
    if (negRect < negEnv)
        negEnv = negRect;
    else
        negEnv -= envCoeff * negEnv;
 
    // Complementary comparators
    const bool clrActive = (input > posEnv);
    const bool prActive  = (input < negEnv);
 
    // First flip-flop
    if (prActive)
        ff1 = true;
    if (clrActive)
        ff1 = false;
 
    // Second flip-flop
    const bool ff1Rise = (ff1 && ! prevFF1);
    prevFF1 = ff1;
 
    if (ff1Rise)
        ff2 = ! ff2;
 
    // Sub-octave square wave
    const float squareOut = (ff2 ? 1.0f : -1.0f) * posEnv;
 
    // Buzz switch
    if (buzz)
    {
        buzzFilter += buzzCoeff * (squareOut - buzzFilter);
        return buzzFilter;
    }
 
    return squareOut;
}
