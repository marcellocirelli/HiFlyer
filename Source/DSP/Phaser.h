/*
  ==============================================================================

    Phaser.h
    Created: 17 Feb 2026 11:41:30pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class Phaser
{
    public:
    Phaser() = default;
    
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample (float input, float modulation) noexcept;
    void setFeedback (float newFeedback) noexcept;
    void setSweepRange (float minFreqHz, float maxFreqHz) noexcept;
    void setActiveStages (int stages) noexcept;
    void setOutputTap (int stage) noexcept;
    
    private:
    static constexpr int numStages = 6;
    int activeStages = numStages;
    
    static constexpr float nominalFreqHz = 1.0f / (juce::MathConstants<float>::twoPi * 1.0e3f * 10.0e-9f);
    
    float coefficient (float freqHz) const noexcept;
    
    float feedback = 0.0f;
    float feedbackState = 0.0f;
    float sweepMinHz = 300.0f;
    float sweepMaxHz = nominalFreqHz;
    
    float sampleRate = 0.0f;
    float z1[numStages] {};
    
    int outputTap = numStages;
};
