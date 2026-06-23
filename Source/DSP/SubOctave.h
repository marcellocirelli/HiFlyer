/*
  ============================================================================

    SubOctave.h

  ============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class SubOctave
{
public:
    SubOctave() = default;

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    // buzz == true applies the low-pass buzz/tone switch behavior.
    float processSample (float input, bool buzz) noexcept;

    // Normalized bipolar fundamental square from the first flip-flop.
    // Use this for ring mod.
    float getCarrier() const noexcept { return fundamentalCarrierOut; }

    // Normalized bipolar octave-down square from the second flip-flop.
    // Use this if growl should divide the sub-octave square further.
    float getSubCarrier() const noexcept { return subCarrierOut; }

private:
    static float msToCoeff (double sr, float ms) noexcept;
    static int msToSamples (double sr, float ms) noexcept;

    void updateComparators (float x) noexcept;
    void setFirstFlipFlop (bool newState) noexcept;

    // ===== Bass-friendly tracking range =====
    static constexpr float kMinInputHz = 24.0f;
    static constexpr float kMaxInputHz = 1800.0f;

    // Input conditioning.
    static constexpr float kInputHighpassMs = 360.0f;
    static constexpr float kTrackingLowpassMs = 0.42f;

    // Envelope follower used for adaptive comparator threshold and output level.
    static constexpr float kEnvelopeAttackMs = 4.0f;
    static constexpr float kEnvelopeReleaseMs = 90.0f;

    // Adaptive threshold.
    static constexpr float kThresholdRatio = 0.18f;
    static constexpr float kThresholdFloor = 0.0018f;
    static constexpr float kThresholdCeiling = 0.22f;

    // Comparator hysteresis.
    static constexpr float kHysteresisRatio = 0.48f;

    // Reject impossibly fast comparator chatter.
    static constexpr float kEdgeBlankingMs = 0.55f;

    // Output behavior.
    static constexpr float kOutputMakeupGain = 3.6f;
    static constexpr float kOutputLimit = 0.98f;
    static constexpr float kOutputSlewMs = 0.85f;
    static constexpr float kCarrierSlewMs = 0.35f;

    // Slow gate follows input envelope so the sub does not click on/off.
    static constexpr float kGateOpenThreshold = 0.0045f;
    static constexpr float kGateCloseThreshold = 0.0020f;
    static constexpr float kGateAttackMs = 1.2f;
    static constexpr float kGateReleaseMs = 65.0f;

    // Hardware buzz switch as low-pass/tone shaping.
    static constexpr float kBuzzLowpassMs = 2.8f;

    double sampleRate = 44100.0;

    float highpassCoeff = 0.0f;
    float lowpassCoeff = 0.0f;
    float envAttackCoeff = 0.0f;
    float envReleaseCoeff = 0.0f;
    float outputSlewCoeff = 0.0f;
    float carrierSlewCoeff = 0.0f;
    float gateAttackCoeff = 0.0f;
    float gateReleaseCoeff = 0.0f;
    float buzzCoeff = 0.0f;

    int minHalfPeriodSamples = 1;
    int maxHalfPeriodSamples = 1;
    int blankingSamples = 1;

    float highpassState = 0.0f;
    float lowpassState = 0.0f;
    float envelope = 0.0f;

    bool positiveComparator = false;
    bool negativeComparator = false;

    bool ff1 = false;
    bool ff2 = false;

    int samplesSinceEdge = 0;

    bool gateOpen = false;
    float gate = 0.0f;

    float fundamentalCarrierOut = 0.0f;
    float subCarrierOut = 0.0f;
    float outputSlew = 0.0f;
    float buzzFilter = 0.0f;
};
