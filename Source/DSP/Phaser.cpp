/*
  ==============================================================================

    Phaser.cpp
    Created: 17 Feb 2026 11:41:30pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "Phaser.h"

void Phaser::prepare(const juce::dsp::ProcessSpec &spec)
{
    sampleRate = spec.sampleRate;
    reset();
}

void Phaser::reset()
{
    feedbackState = 0.0f;
    std::fill (std::begin (z1), std::end (z1), 0.0f);
}

void Phaser::setFeedback(float newFeedback) noexcept
{
    feedback = juce::jlimit (0.0f, 0.95f, newFeedback);
}

void Phaser::setSweepRange(float minFreqHz, float maxFreqHz) noexcept
{
    sweepMinHz = juce::jlimit(20.0f, 20000.0f, minFreqHz);
    sweepMaxHz = juce::jlimit(sweepMinHz, 20000.0f, maxFreqHz);
}

void Phaser::setActiveStages (int stages) noexcept
{
    activeStages = juce::jlimit(1, numStages, stages);
}

void Phaser::setOutputTap(int stage) noexcept
{
    outputTap = juce::jlimit(1, numStages, stage);
}

float Phaser::coefficient(float freqHz) const noexcept
{
    const float w = std::tan (juce::MathConstants<float>::pi * freqHz / static_cast<float> (sampleRate));
    return (w - 1.0f) / (w + 1.0f);
}

float Phaser::processSample(float input, float modulation) noexcept
{
    const float mod = juce::jlimit(0.0f, 1.0f, modulation);
    const float freq = sweepMinHz * std::pow(sweepMaxHz / sweepMinHz, mod);
    const float a = coefficient(freq);
    
    float x = input + feedbackState * feedback;
    float tappedOutput = 0.0f;
    
    for (int i = 0; i < numStages; ++i)
    {
        const float y = a * x + z1[i];
        z1[i] = x - a * y;
        x = y;
        
        if (i == outputTap - 1)
            tappedOutput = x;
    }
    
    feedbackState = x;
    return tappedOutput;
}
