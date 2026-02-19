/*
  ==============================================================================

    LFO.h
    Created: 18 Feb 2026 1:18:00pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>

class LFO
{
    public:
    enum class Waveform
    {
        SlowSine,
        FastSine,
        RampUpSine,
        RampDownSine,
        LinearRampUp,
        LinearRampDown
    };
    
    LFO() = default;
    
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample() noexcept;
    
    void setWaveform (Waveform newWaveform);
    void setSpeed (float speed) noexcept;
    void setDepth (float depth) noexcept;
    
    private:
    float frequencyForSpeed (float s) const noexcept;
    
    double sampleRate = 0.0;
    
    Waveform waveform = Waveform::SlowSine;
    float speed = 0.0f;
    float depth = 1.0f;
    
    double phase = 0.0;
    
    static constexpr float slowMaxHz =  1.14f;
    static constexpr float fastMaxHz = 6.25f;
};
