/*
  ============================================================================

    SustainFuzz.h

  ============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class SustainFuzz
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    float processSample (float input, float attackEnv) noexcept;

private:
    double sampleRate = 48000.0;

    // Output HPF state: C22[1µF] × R68[470Ω] ≈ 338Hz
    float hpfState  = 0.0f;
    float hpfCoeff  = 0.0f;

    // Output LPF state: C21[22kp] × R64[22k] ≈ 329Hz
    float lpfState  = 0.0f;
    float lpfCoeff  = 0.0f;

    static constexpr float kHpfHz = 338.0f;
    static constexpr float kLpfHz = 329.0f;

    // Small floor so ceiling never hits zero
    static constexpr float kCeilingFloor = 1e-4f;
    static constexpr float kGain = 35.0f;
};
