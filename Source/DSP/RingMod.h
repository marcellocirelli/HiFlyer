/*
  ==============================================================================

    RingMod.h
    Created: 24 Mar 2026 11:01:59am
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class RingMod
{
    public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample(float input, float amount) noexcept;
    
    private:
    static constexpr float kSoftness = 0.0001f;
};
