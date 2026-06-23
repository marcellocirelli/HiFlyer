/*
  ==============================================================================

    TopBoost.h
    Created: 24 Feb 2026 12:45:48pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class TopBoost
{
public:
    TopBoost() = default;
    
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample (float input, float boostAmount) noexcept;
    
private:
    void computeCoefficients (double sampleRate);
    
    juce::dsp::IIR::Filter<float> inputFilter;
    juce::dsp::IIR::Filter<float> boostFilter;
    
    // For bilinear transform. Calculated from original component values
    static constexpr double inputZero  = 4.95e-4;
    static constexpr double inputDCSum = 36300.0;
    static constexpr double inputPole  = 1.6335e-3;
    static constexpr double boostNumS1 = 1.5965e-3;
    static constexpr double boostDenS1 = 9.65e-5;
    static constexpr double boostS2    = 2.3265e-9;
    static constexpr double R70 = 3300.0;
};
