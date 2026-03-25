/*
  ==============================================================================

    SubOctave.h
    Created: 23 Mar 2026 4:07:20pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class SubOctave
{
    public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample (float input, bool buzz) noexcept;
    
    private:
    double sampleRate = 0.0;
    
    // Pre-filter
    float preFilter      = 0.0f;
    float preFilterCoeff = 0.0f;
 
    // Envelope followers (A7)
    float posEnv   = 0.0f;
    float negEnv   = 0.0f;
    float envCoeff = 0.0f;
 
    // Flip-flop stages
    bool  ff1      = false;
    bool  prevFF1  = false;
    bool  ff2      = false;
 
    // Buzz LPF state
    float buzzFilter = 0.0f;
    float buzzCoeff  = 0.0f;
 
    static constexpr float kPreFilterMs = 1.0f;     // R42[10k] × C15[100n]
    static constexpr float kEnvTimeMs = 33.0f;    // R44[33k] × C17[1µ]
    static constexpr float kBuzzCutoffMs = 1.0f;    // 100Ω × 10µF
};
