/*
  ============================================================================

    SubOctave.cpp

    This replicates the sub octave module. This module directly replicates the behavior
    of the dual envelope follower/comparator circuit on the original hardware.
    The circuit detects the input fundamental, and generates a square wave / 2.
    The buzz switch effectively converts the square wave to a triangle wave by
    applying a low pass filter to the square wave.
 
  ============================================================================
*/

#include "SubOctave.h"
#include <algorithm>
#include <cmath>

float SubOctave::msToCoeff (double sr, float ms) noexcept
{
    ms = std::max (0.001f, ms);
    return 1.0f - std::exp (-1.0f / (static_cast<float> (sr) * ms * 0.001f));
}

int SubOctave::msToSamples (double sr, float ms) noexcept
{
    return std::max (1, static_cast<int> (std::round (sr * ms * 0.001)));
}

void SubOctave::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;

    highpassCoeff = msToCoeff (sampleRate, kInputHighpassMs);
    lowpassCoeff  = msToCoeff (sampleRate, kTrackingLowpassMs);

    envAttackCoeff  = msToCoeff (sampleRate, kEnvelopeAttackMs);
    envReleaseCoeff = msToCoeff (sampleRate, kEnvelopeReleaseMs);

    outputSlewCoeff  = msToCoeff (sampleRate, kOutputSlewMs);
    carrierSlewCoeff = msToCoeff (sampleRate, kCarrierSlewMs);

    gateAttackCoeff  = msToCoeff (sampleRate, kGateAttackMs);
    gateReleaseCoeff = msToCoeff (sampleRate, kGateReleaseMs);

    buzzCoeff = msToCoeff (sampleRate, kBuzzLowpassMs);

    minHalfPeriodSamples = std::max (1, static_cast<int> (sampleRate / (kMaxInputHz * 2.0f)));
    maxHalfPeriodSamples = std::max (1, static_cast<int> (sampleRate / (kMinInputHz * 2.0f)));
    blankingSamples = msToSamples (sampleRate, kEdgeBlankingMs);

    reset();
}

void SubOctave::reset()
{
    highpassState = 0.0f;
    lowpassState = 0.0f;
    envelope = 0.0f;

    positiveComparator = false;
    negativeComparator = false;

    ff1 = false;
    ff2 = false;

    samplesSinceEdge = maxHalfPeriodSamples;

    gateOpen = false;
    gate = 0.0f;

    subCarrierOut = 0.0f;
    outputSlew = 0.0f;
    buzzFilter = 0.0f;
}

void SubOctave::setFirstFlipFlop (bool newState) noexcept
{
    if (ff1 == newState)
        return;

    const int halfPeriod = samplesSinceEdge;
    samplesSinceEdge = 0;

    ff1 = newState;

    // Only clock the divide-by-two stage if the first-stage transition is
    // within a reasonable half-cycle range
    if (halfPeriod >= minHalfPeriodSamples && halfPeriod <= maxHalfPeriodSamples)
    {
        if (ff1)
            ff2 = ! ff2;
    }
}

void SubOctave::updateComparators (float x) noexcept
{
    if (samplesSinceEdge < blankingSamples)
        return;

    const float threshold = juce::jlimit (kThresholdFloor,
                                          kThresholdCeiling,
                                          envelope * kThresholdRatio);

    const float releaseThreshold = threshold * kHysteresisRatio;

    // Complementary Schmitt comparators
    // Positive comparator clears ff1
    if (! positiveComparator && x >= threshold)
    {
        positiveComparator = true;
        negativeComparator = false;
        setFirstFlipFlop (false);
    }
    else if (positiveComparator && x < releaseThreshold)
    {
        positiveComparator = false;
    }

    // Negative comparator sets ff1
    if (! negativeComparator && x <= -threshold)
    {
        negativeComparator = true;
        positiveComparator = false;
        setFirstFlipFlop (true);
    }
    else if (negativeComparator && x > -releaseThreshold)
    {
        negativeComparator = false;
    }
}

float SubOctave::processSample (float input, bool buzz) noexcept
{
    // Bass friendly DC removal
    highpassState += highpassCoeff * (input - highpassState);
    const float highpassed = input - highpassState;

    // For reduction of string/fret/pick harmonics
    lowpassState += lowpassCoeff * (highpassed - lowpassState);
    const float tracked = lowpassState;

    // Perfect rectifier style follower with separate attack/release.
    const float rectified = std::abs (tracked);
    const float envCoeff = rectified > envelope ? envAttackCoeff : envReleaseCoeff;
    envelope += envCoeff * (rectified - envelope);

    ++samplesSinceEdge;
    if (samplesSinceEdge > maxHalfPeriodSamples * 4)
        samplesSinceEdge = maxHalfPeriodSamples * 4;

    updateComparators (tracked);

    // Simple level gate
    if (! gateOpen && envelope >= kGateOpenThreshold)
        gateOpen = true;
    else if (gateOpen && envelope <= kGateCloseThreshold)
        gateOpen = false;

    const float gateTarget = gateOpen ? 1.0f : 0.0f;
    const float gateCoeff = gateTarget > gate ? gateAttackCoeff : gateReleaseCoeff;
    gate += gateCoeff * (gateTarget - gate);

    const float subRaw = ff2 ? 1.0f : -1.0f;

    subCarrierOut += carrierSlewCoeff * ((gateOpen ? subRaw : 0.0f) - subCarrierOut);

    const float levelProportional = juce::jlimit (-kOutputLimit,
                                                  kOutputLimit,
                                                  subCarrierOut * envelope * kOutputMakeupGain * gate);

    outputSlew += outputSlewCoeff * (levelProportional - outputSlew);

    if (buzz)
    {
        buzzFilter += buzzCoeff * (outputSlew - buzzFilter);
        return buzzFilter;
    }

    return outputSlew;
}
