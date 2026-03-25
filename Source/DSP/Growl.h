/*
  ==============================================================================

    Growl.h
    Created: 24 Mar 2026 8:22:49pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class Growl
{
    public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample(float subOctaveInput, int mode) noexcept;
    
    private:
    float prevSample = 0.0f;
    bool ff1 = false;
    bool prevFF1 = false;
    bool ff2 = false;
};
