/*
  ==============================================================================

    LFO.cpp
    Created: 18 Feb 2026 1:18:00pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "LFO.h"
#include <cmath>

void LFO::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

void LFO::reset()
{
    freePhase = 0.0;
    trigPhase = 0.0;
    ramp = 0.0f;
    rampActive = false;
}

void LFO::setWaveform (Waveform newWaveform)
{
    waveform = newWaveform;
}

void LFO::setSpeed (float newSpeed) noexcept
{
    speed = juce::jlimit (0.0f, 1.0f, newSpeed);
}

void LFO::setDepth (float newDepth) noexcept
{
    depth = juce::jlimit (0.0f, 1.0f, newDepth);
}

float LFO::processSample (bool attackTrigger, bool decayTrigger, float rampAmount) noexcept
{
    if (waveform == Waveform::SlowSine || waveform == Waveform::FastSine)
    {
        const float maxHz = (waveform == Waveform::SlowSine) ? slowMaxHz : fastMaxHz;
        const float freq = maxHz * speed;
        const double phaseInc = freq / sampleRate;
 
        const float sine = static_cast<float> (std::sin (juce::MathConstants<double>::twoPi * freePhase));
        const float out = (sine * 0.5f + 0.5f) * depth;
 
        freePhase += phaseInc;
        if (freePhase >= 1.0)
            freePhase -= 1.0;
 
        return out;
    }
 
    if (decayTrigger)
    {
        ramp       = 0.0f;
        rampActive = false;
    }
 
    if (attackTrigger)
    {
        ramp       = 0.0f;
        rampActive = true;
        trigPhase  = 0.0;
    }
 
    // Advance ramp
    if (rampActive && ramp < 1.0f)
    {
        const float rampMs = kMinRampMs * std::pow (kMaxRampMs / kMinRampMs, rampAmount);
        const float rampInc = 1.0f / (static_cast<float> (sampleRate) * rampMs * 0.001f);
        ramp = std::min (1.0f, ramp + rampInc);
    }
 
    // Advance triggered sine phase (at FastSine rate)
    const float freq = fastMaxHz * speed;
    const double phaseInc = freq / sampleRate;
    const float sine = static_cast<float> (std::sin (juce::MathConstants<double>::twoPi * trigPhase));
 
    trigPhase += phaseInc;
    if (trigPhase >= 1.0)
        trigPhase -= 1.0;
 
    // Output
    switch (waveform)
    {
        case Waveform::RampUpSine:
            return (sine * 0.5f + 0.5f) * ramp * depth;
 
        case Waveform::RampDownSine:
            return (sine * 0.5f + 0.5f) * (1.0f - ramp) * depth;
 
        case Waveform::LinearRampUp:
            return ramp * depth;
 
        case Waveform::LinearRampDown:
            return (1.0f - ramp) * depth;
 
        default:
            return 0.0f;
    }
}
