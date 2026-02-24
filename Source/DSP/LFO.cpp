/*
  ==============================================================================

    LFO.cpp
    Created: 18 Feb 2026 1:18:00pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "LFO.h"

void LFO::prepare(const juce::dsp::ProcessSpec &spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

void LFO::reset()
{
    phase = 0.0;
}

void LFO::setWaveform(Waveform newWaveform)
{
    waveform = newWaveform;
}

void LFO::setSpeed(float newSpeed) noexcept
{
    speed = juce::jlimit(0.0f, 1.0f, newSpeed);
}

void LFO::setDepth(float newDepth) noexcept
{
    depth = juce::jlimit(0.0f, 1.0f, newDepth);
}

float LFO::frequencyForSpeed(float s) const noexcept
{
    const bool slow = (waveform == Waveform::SlowSine);
    const float maxHz = slow ? slowMaxHz : fastMaxHz;
    
    return maxHz * s;
}

float LFO::processSample() noexcept
{
    switch (waveform)
    {
        case Waveform::SlowSine:
        case Waveform::FastSine:
        {
            const float freq = frequencyForSpeed (speed);
            const double phaseInc = freq / sampleRate;

            const float sine = static_cast<float> (std::sin (juce::MathConstants<double>::twoPi * phase));
            const float out = (sine * 0.5f + 0.5f) * depth;

            phase += phaseInc;
            if (phase >= 1.0)
                phase -= 1.0;

            return out;
        }

        // Triggered waveforms, return center value for now
        case Waveform::RampUpSine:
        case Waveform::RampDownSine:
        case Waveform::LinearRampUp:
        case Waveform::LinearRampDown:
            return 0.5f * depth;
    }

    return 0.0f;
}
