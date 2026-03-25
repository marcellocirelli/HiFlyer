/*
  ==============================================================================

    EnvelopeDetector.cpp

  ==============================================================================
*/

#include "EnvelopeDetector.h"
#include <cmath>

static float msToCoeff (double sampleRate, float ms) noexcept
{
    return 1.0f - std::exp (-1.0f / (static_cast<float> (sampleRate) * ms * 0.001f));
}

void EnvelopeDetector::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    fastEnv.prepare (spec);
    fastEnv.setAttackTime (kFastAttackMs);
    fastEnv.setReleaseTime (kFastReleaseMs);

    slowEnv.prepare (spec);
    slowEnv.setAttackTime (kSlowAttackMs);
    slowEnv.setReleaseTime (kSlowReleaseMs);

    holdoffSamples = static_cast<int> (sampleRate * kSoloHoldoffMs * 0.001);

    reset();
}

void EnvelopeDetector::reset()
{
    fastEnv.reset();
    slowEnv.reset();
    gateState      = false;
    holdoffCounter = 0;
    decayEnv       = 0.0f;
    peakHold       = 0.0f;
    attackEnv      = 0.0f;
}

EnvelopeOutput EnvelopeDetector::processSample (float rawInput, float boostedInput,
                                                 float fallTime, float riseTime,
                                                 bool soloMode) noexcept
{
    EnvelopeOutput out;

    // Transient Detection

    const float rectified = std::abs (rawInput);
    const float fast = fastEnv.processSample (0, rectified);
    const float slow = slowEnv.processSample (0, rectified);
    const float ratio = (slow > 1e-6f) ? (fast / slow) : 0.0f;
    const float threshold = soloMode ? kSoloAttackRatio : kStrumAttackRatio;

    if (holdoffCounter > 0)
        --holdoffCounter;

    // --- Note end: signal died ---
    if (gateState && fast < kNoiseFloor)
    {
        gateState        = false;
        out.decayTrigger = true;

        // Reset fuzz envelope
        peakHold  = 0.0f;
        attackEnv = 0.0f;
    }

    // --- Note start: transient detected ---
    if (ratio > threshold)
    {
        if (soloMode)
        {
            // Solo: retrigger any time, holdoff prevents chatter
            if (holdoffCounter == 0)
            {
                if (gateState)
                {
                    // Previous note ended — reset fuzz envelope
                    out.decayTrigger = true;
                    peakHold  = 0.0f;
                    attackEnv = 0.0f;
                }

                gateState         = true;
                out.attackTrigger = true;
                holdoffCounter    = holdoffSamples;

                // Decay envelope: jump to 1.0 on new note
                decayEnv = 1.0f;
            }
        }
        else
        {
            // Strum: gate must be closed first
            if (! gateState)
            {
                gateState         = true;
                out.attackTrigger = true;

                decayEnv = 1.0f;
            }
        }
    }

    out.gate = gateState;

    // Decay Envelope

    const float decayMs    = kMinTimeMs * std::pow (kMaxTimeMs / kMinTimeMs, fallTime);
    const float decayCoeff = msToCoeff (sampleRate, decayMs);
    decayEnv -= decayCoeff * decayEnv;

    out.decayEnvelope = decayEnv;

    // Attack Envelope
    if (gateState)
    {
        // Peak detector (C19): track largest excursion of boosted signal
        const float boostedLevel = std::abs (boostedInput);
        if (boostedLevel > peakHold)
            peakHold = boostedLevel;

        // Attack envelope (C20): rise toward peak at user rate
        if (riseTime < 0.001f)
        {
            attackEnv = peakHold;
        }
        else
        {
            const float riseMs    = kMinTimeMs * std::pow (kMaxTimeMs / kMinTimeMs, riseTime);
            const float riseCoeff = msToCoeff (sampleRate, riseMs);
            attackEnv += riseCoeff * (peakHold - attackEnv);
        }
    }

    out.attackEnvelope = attackEnv;

    return out;
}
