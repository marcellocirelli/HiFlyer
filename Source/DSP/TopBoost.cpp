/*
  ==============================================================================

    TopBoost.cpp
    Created: 24 Feb 2026 12:45:48pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "TopBoost.h"

void TopBoost::prepare(const juce::dsp::ProcessSpec &spec)
{
    inputFilter.prepare(spec);
    boostFilter.prepare(spec);
    computeCoefficients(spec.sampleRate);
    reset();
}

void TopBoost::reset()
{
    inputFilter.reset();
    boostFilter.reset();
}

float TopBoost::processSample(float input, float boostAmount) noexcept
{
    const float boosted = boostFilter.processSample(inputFilter.processSample(input));
    return input + boostAmount * (boosted - input);
}

void TopBoost::computeCoefficients(double sampleRate)
{
    const double c = 2.0 * sampleRate;
    const double c2 = c * c;
    
    inputFilter.coefficients = new juce::dsp::IIR::Coefficients<float>(
        static_cast<float>(R70 + R70 * inputZero * c),
        static_cast<float>(R70 - R70 * inputZero * c),
        static_cast<float>(inputDCSum + inputPole * c),
        static_cast<float>(inputDCSum - inputPole * c));
     
    boostFilter.coefficients = new juce::dsp::IIR::Coefficients<float>(
        static_cast<float>(1.0 + boostNumS1 * c + boostS2 * c2),
        static_cast<float>(2.0 - 2.0 * boostS2 * c2),
        static_cast<float>(1.0 - boostNumS1 * c + boostS2 * c2),
        static_cast<float>(1.0 + boostDenS1 * c + boostS2 * c2),
        static_cast<float>(2.0 - 2.0 * boostS2 * c2),
        static_cast<float>(1.0 - boostDenS1 * c + boostS2 * c2));
}
