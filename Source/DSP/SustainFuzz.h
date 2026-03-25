/*
  ==============================================================================

    SustainFuzz.h
    Created: 24 Mar 2026 1:04:24pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class SustainFuzz
{
    public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample (float input, float attackEnvelope, float fuzzLevel) noexcept;
    
    private:
    double sampleRate = 0.0;
    
    float lpfState = 0.0f;
    float lpfCoeff = 0.0f;
    
    static constexpr float kGain = 300.0f;
    static constexpr float kLpfHz = 329.0f;
    static constexpr float kOtaGain = 5.0f;
};
