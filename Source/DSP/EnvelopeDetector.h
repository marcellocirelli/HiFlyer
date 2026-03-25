/*
  ==============================================================================

    EnvelopeDetector.h


  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

struct EnvelopeOutput
{
    bool  gate           = false;
    bool  attackTrigger  = false;
    bool  decayTrigger   = false;
    float decayEnvelope  = 0.0f;
    float attackEnvelope = 0.0f;
};

class EnvelopeDetector
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    EnvelopeOutput processSample (float rawInput, float boostedInput,
                                  float fallTime, float riseTime,
                                  bool soloMode) noexcept;

private:
    double sampleRate = 48000.0;

    // Transient detection state
    juce::dsp::BallisticsFilter<float> fastEnv;
    juce::dsp::BallisticsFilter<float> slowEnv;
    bool  gateState    = false;
    int   holdoffCounter = 0;
    int   holdoffSamples = 0;

    // Decay envelope state
    float decayEnv = 0.0f;

    // Attack envelope state
    float peakHold  = 0.0f;
    float attackEnv = 0.0f;

    // Ballistics follower times
    static constexpr float kFastAttackMs  = 5.0f;
    static constexpr float kFastReleaseMs = 10.0f;
    static constexpr float kSlowAttackMs  = 30.0f;
    static constexpr float kSlowReleaseMs = 150.0f;

    // Attack sensitivity — fast/slow ratio
    static constexpr float kSoloAttackRatio  = 1.8f;
    static constexpr float kStrumAttackRatio = 3.0f;

    // Note-end: fast envelope below this = silence
    static constexpr float kNoiseFloor = 0.001f;

    // Solo holdoff debounce
    static constexpr float kSoloHoldoffMs = 30.0f;

    // Decay/attack time ranges: 10µF × 500kΩ log pot
    static constexpr float kMinTimeMs = 5.0f;
    static constexpr float kMaxTimeMs = 5000.0f;
};
