/*
  ==============================================================================

    Growl.cpp
    Created: 24 Mar 2026 8:22:49pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "Growl.h"
#include <cmath>

void Growl::prepare(const juce::dsp::ProcessSpec &spec)
{
    reset();
}

void Growl::reset()
{
    prevSample = 0.0f;
    ff1 = false;
    prevFF1 = false;
    ff2 = false;
}

float Growl::processSample(float subOctaveInput, int mode) noexcept
{
    if (mode == 1)
        return 0.0f;
    
    const bool crossed = (prevSample <= 0.0f && subOctaveInput > 0.0f);
    prevSample = subOctaveInput;
    // FF1
    if (crossed)
        ff1 = ! ff1;
    
    // FF2
    if (mode == 2)
    {
        const bool ff1Rise = (ff1 && ! prevFF1);
        
        if (ff1Rise)
            ff2 = ! ff2;
    }
    
    prevFF1 = ff1;
    
    const float amplitude = std::abs (subOctaveInput);
    const bool state = (mode == 0) ? ff1 : ff2;
    return (state ? 1.0f : -1.0f) * amplitude;
}
