/*
  ============================================================================

    SustainFuzz.cpp

  ============================================================================
*/

#include "SustainFuzz.h"
#include <cmath>
#include <algorithm>

void SustainFuzz::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 48000.0;

    // Output HPF: C22[1µF] × R68[470Ω] ≈ 338Hz
    const float wHpf = juce::MathConstants<float>::twoPi * kHpfHz / static_cast<float> (sampleRate);
    hpfCoeff = 1.0f / (1.0f + wHpf);

    // Output LPF: C21[22kp] × R64[22k] ≈ 329Hz
    const float wLpf = juce::MathConstants<float>::twoPi * kLpfHz / static_cast<float> (sampleRate);
    lpfCoeff = wLpf / (1.0f + wLpf);

    reset();
}

void SustainFuzz::reset()
{
    hpfState = 0.0f;
    lpfState = 0.0f;
}

float SustainFuzz::processSample (float input, float attackEnv) noexcept
{
    //--------------------------------------------------------------------------
    // Q6 common-base:
    // attackEnvelope (A12 output) sets the collector voltage ceiling.
    // Signal clips symmetrically against ±ceiling.
    // Low ceiling = hard clip at low amplitude.
    // High ceiling = proportional output at higher amplitude.
    //--------------------------------------------------------------------------
    const float ceiling = std::max (attackEnv, kCeilingFloor);
    const float amplified = input * kGain;
    const float clipped = std::clamp (amplified, -ceiling, ceiling);

    //--------------------------------------------------------------------------
    // Output HPF: C22[1µF] × R68[470Ω] — AC coupling from base
    //--------------------------------------------------------------------------
    const float hpfOut = hpfCoeff * (hpfState + clipped);
    hpfState = hpfOut - clipped;

    //--------------------------------------------------------------------------
    // Output LPF: C21[22kp] × R64[22k] ≈ 329Hz
    //--------------------------------------------------------------------------
    lpfState += lpfCoeff * (hpfOut - lpfState);

    return lpfState;
}
