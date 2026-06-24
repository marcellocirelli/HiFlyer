/*
  ==============================================================================

    EnvelopeDetector.cpp

    This module detects attack and decay transients for various uses across the plugin.
    Attack detection is used for fall time, sustain fuzz rise time, and waveform triggering in the phaser
    Decay detection is used for resetting
 
  ==============================================================================
*/

#include "EnvelopeDetector.h"
#include <cmath>

float EnvelopeDetector::msToCoeff (double sr, float ms) noexcept
{
    ms = std::max (0.001f, ms);
    return 1.0f - std::exp (-1.0f / (static_cast<float> (sr) * ms * 0.001f));
}

int EnvelopeDetector::msToSamples (double sr, float ms) noexcept
{
    return std::max (1, static_cast<int> (std::round (sr * ms * 0.001)));
}

float EnvelopeDetector::mapTimeControl (float normalised) noexcept
{
    const float x = juce::jlimit (0.0f, 1.0f, normalised);
    const float curved = x * x;
    return kMinTimeMs + curved * (kMaxTimeMs - kMinTimeMs);
}

float EnvelopeDetector::onePoleAttackRelease (float input, float& state,
                                               float attackCoeff,
                                               float releaseCoeff) noexcept
{
    const float coeff = (input > state) ? attackCoeff : releaseCoeff;
    state += coeff * (input - state);
    return state;
}

void EnvelopeDetector::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 44100.0;

    levelAttackCoeff = msToCoeff (sampleRate, kLevelAttackMs);
    levelReleaseCoeff = msToCoeff (sampleRate, kLevelReleaseMs);
    fastAttackCoeff = msToCoeff (sampleRate, kFastAttackMs);
    fastReleaseCoeff = msToCoeff (sampleRate, kFastReleaseMs);
    slowAttackCoeff = msToCoeff (sampleRate, kSlowAttackMs);
    slowReleaseCoeff = msToCoeff (sampleRate, kSlowReleaseMs);

    soloHoldoffSamples = msToSamples (sampleRate, kSoloHoldoffMs);
    strumHoldoffSamples = msToSamples (sampleRate, kStrumHoldoffMs);
    closeConfirmSamples = msToSamples (sampleRate, kCloseConfirmMs);

    reset();
}

void EnvelopeDetector::reset()
{
    levelEnv = 0.0f;
    fastEnv = 0.0f;
    slowEnv = 0.0f;
    previousLevel = 0.0f;

    // Safe defaults: with no detected event, the audio path is not muted
    riseEnv = 1.0f;
    fallEnv = 1.0f;

    gateState = false;
    holdoffCounter = 0;
    closeCounter = 0;
}

EnvelopeOutput EnvelopeDetector::processSample (float rawInput,
                                                 float boostedInput,
                                                 float fallTime,
                                                 float riseTime,
                                                 bool soloMode) noexcept
{
    juce::ignoreUnused (boostedInput);

    EnvelopeOutput out;

    const float rectified = std::abs (rawInput);

    const float level = onePoleAttackRelease (rectified, levelEnv, levelAttackCoeff, levelReleaseCoeff);
    const float fast  = onePoleAttackRelease (rectified, fastEnv, fastAttackCoeff, fastReleaseCoeff);
    const float slow  = onePoleAttackRelease (rectified, slowEnv, slowAttackCoeff, slowReleaseCoeff);

    const float transientAmount = fast - slow;
    const float positiveSlope = std::max (0.0f, level - previousLevel);
    previousLevel = level;

    if (holdoffCounter > 0)
        --holdoffCounter;

    const float transientThreshold = soloMode ? kSoloTransient : kStrumTransient;
    const float slopeThreshold = soloMode ? kSoloSlope : kStrumSlope;
    const int holdoffSamples = soloMode ? soloHoldoffSamples : strumHoldoffSamples;

    const bool aboveNoise = level > kNoiseFloor;
    const bool enoughLevel = level > kOpenThreshold;
    const bool transientDetected = aboveNoise
                                && enoughLevel
                                && transientAmount > transientThreshold
                                && positiveSlope > slopeThreshold
                                && holdoffCounter == 0;

    if (gateState)
    {
        if (level < kCloseThreshold)
        {
            if (++closeCounter >= closeConfirmSamples)
            {
                gateState = false;
                out.decayTrigger = true;
                closeCounter = 0;
            }
        }
        else
        {
            closeCounter = 0;
        }
    }

    if (transientDetected)
    {
        if (gateState)
            out.decayTrigger = true;

        gateState = true;
        out.attackTrigger = true;
        holdoffCounter = holdoffSamples;
        closeCounter = 0;

        // Start a new event. Fall begins at full level and decays. Rise either
        // bypasses to 1.0 at minimum, or swells from silence toward full fuzz.
        fallEnv = 1.0f;
        riseEnv = (riseTime <= kRiseBypassEpsilon) ? 1.0f : 0.0f;
    }

    out.gate = gateState;

    if (fallTime >= 0.999f)
    {
        fallEnv = 1.0f; // max fall = no decay / off
    }
    else
    {
        const float fallMs = mapTimeControl (fallTime);
        const float fallStep = 1.0f / static_cast<float> (msToSamples (sampleRate, fallMs));
        fallEnv = std::max (0.0f, fallEnv - fallStep);
    }

    out.decayEnvelope = juce::jlimit (0.0f, 1.0f, fallEnv);

    if (riseTime <= kRiseBypassEpsilon)
    {
        riseEnv = 1.0f;
    }
    else if (gateState)
    {
        const float riseMs = mapTimeControl (riseTime);
        const float riseStep = 1.0f / static_cast<float> (msToSamples (sampleRate, riseMs));
        riseEnv = std::min (1.0f, riseEnv + riseStep);
    }
    else
    {
        // The next attack event resets this to zero. Between events, leave it at
        // full level so the fuzz path is normal when rise is not actively swelling.
        riseEnv = 1.0f;
    }

    out.attackEnvelope = juce::jlimit (0.0f, 1.0f, riseEnv);
    return out;
}
