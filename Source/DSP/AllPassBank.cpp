/*
  ==============================================================================

    Phaser.cpp
    Created: 17 Feb 2026 11:41:30pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "AllPassBank.h"

void AllPassBank::prepare(const juce::dsp::ProcessSpec &spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

void AllPassBank::reset()
{
    feedbackState = 0.0f;
    std::fill (std::begin (z1), std::end (z1), 0.0f);
}

void AllPassBank::setFeedback(float newFeedback) noexcept
{
    feedback = juce::jlimit (0.0f, 0.95f, newFeedback);
}

void AllPassBank::setSweepRange(float minFreqHz, float maxFreqHz) noexcept
{
    sweepMinHz = juce::jlimit(20.0f, 20000.0f, minFreqHz);
    sweepMaxHz = juce::jlimit(sweepMinHz, 20000.0f, maxFreqHz);
}

float AllPassBank::coefficient(float freqHz) const noexcept
{
    const float w = std::tan (juce::MathConstants<float>::pi * freqHz / static_cast<float> (sampleRate));
    return (w - 1.0f) / (w + 1.0f);
}

float AllPassBank::processSample(float input, float modulation) noexcept
{
    const float mod = juce::jlimit(0.0f, 1.0f, modulation);
    const float freq = sweepMaxHz * std::pow(sweepMinHz / sweepMaxHz, mod);
    const float a = coefficient(freq);
    
    float x = input + feedbackState * feedback;
    
    for (int i = 0; i < numStages; ++i)
    {
        const float y = a * x + z1[i];
        z1[i] = x - a * y;
        x = y;
    }
    
    feedbackState = x;
    return x;
}
