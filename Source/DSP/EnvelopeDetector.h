/*
  ==============================================================================

    EnvelopeDetector.h


  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

struct EnvelopeOutput
{
    bool attackTrigger = false;
    bool decayTrigger  = false;
    bool gate          = false;

    // Normalised musical envelopes.
    // riseEnvelope: 1.0 at minimum rise time; otherwise 0 -> 1 after each attack.
    // fallEnvelope: triggered decay, intended to be applied globally before effects.
    float attackEnvelope = 1.0f;
    float decayEnvelope  = 1.0f;
};

class EnvelopeDetector
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    EnvelopeOutput processSample (float rawInput,
                                  float boostedInput,
                                  float fallTime,
                                  float riseTime,
                                  bool soloMode) noexcept;

private:
    static constexpr float kMinTimeMs = 1.0f;
    static constexpr float kMaxTimeMs = 8000.0f;

    static constexpr float kLevelAttackMs  = 0.6f;
    static constexpr float kLevelReleaseMs = 28.0f;
    static constexpr float kFastAttackMs   = 0.15f;
    static constexpr float kFastReleaseMs  = 14.0f;
    static constexpr float kSlowAttackMs   = 7.5f;
    static constexpr float kSlowReleaseMs  = 95.0f;

    static constexpr float kNoiseFloor       = 0.00045f;
    static constexpr float kOpenThreshold    = 0.0032f;
    static constexpr float kCloseThreshold   = 0.0012f;
    static constexpr float kSoloTransient    = 0.0045f;
    static constexpr float kStrumTransient   = 0.0095f;
    static constexpr float kSoloSlope        = 0.0025f;
    static constexpr float kStrumSlope       = 0.0060f;

    static constexpr float kSoloHoldoffMs    = 24.0f;
    static constexpr float kStrumHoldoffMs   = 95.0f;
    static constexpr float kCloseConfirmMs   = 18.0f;

    // Below this control value, rise is treated as completely bypassed so the fuzz
    // behaves exactly like a normal always-on fuzz pedal.
    static constexpr float kRiseBypassEpsilon = 0.0015f;

    static float msToCoeff (double sr, float ms) noexcept;
    static float mapTimeControl (float normalised) noexcept;
    static int msToSamples (double sr, float ms) noexcept;

    float onePoleAttackRelease (float input, float& state, float attackCoeff, float releaseCoeff) noexcept;

    double sampleRate = 44100.0;

    float levelEnv = 0.0f;
    float fastEnv = 0.0f;
    float slowEnv = 0.0f;
    float previousLevel = 0.0f;

    float riseEnv = 1.0f;
    float fallEnv = 1.0f;

    bool gateState = false;
    int holdoffCounter = 0;
    int closeCounter = 0;

    int soloHoldoffSamples = 1;
    int strumHoldoffSamples = 1;
    int closeConfirmSamples = 1;

    float levelAttackCoeff = 0.0f;
    float levelReleaseCoeff = 0.0f;
    float fastAttackCoeff = 0.0f;
    float fastReleaseCoeff = 0.0f;
    float slowAttackCoeff = 0.0f;
    float slowReleaseCoeff = 0.0f;
};
