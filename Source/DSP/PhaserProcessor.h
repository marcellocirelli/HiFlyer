/*
  ==============================================================================

    PhaserProcessor.h
    Created: 18 Feb 2026 11:24:31am
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include "Phaser.h"

class PhaserProcessor
{
    public:
    enum class Treatment
    {
        Vibrato,
        Phasing1,
        Phasing2,
        Waa,
        Waw,
        Meow
    };
    
    PhaserProcessor() = default;
    
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    float processSample (float input, float modulation) noexcept;
    void setTreatment (Treatment newTreatment) noexcept;
    // Feedback amount based on schematic resistors
    void setFeedback (float amount) noexcept;
    // Audio to bow strings for Waa/Waw/Meow
    void setAudioToModDepth (float depth) noexcept;
    
    private:
    void applyTreatment() noexcept;
    
    Phaser upperBank;
    Phaser lowerBank;
    
    Treatment treatment = Treatment::Vibrato;
    
    float feedbackAmount = 0.0f;
    float audioToModDepth = 0.0f;
    
    bool feedbackEnabled = false;
    bool audioToMod = false;
    bool invertLowerMod = false;
    int lowerStages = 6;
    int lowerOutputTap = 6;
    
    enum class MixMode { WetOnly, DryPlusWet, DryHighGainPlusWet, UpperPlusWet };
    MixMode mixMode = MixMode::WetOnly;
    
    static constexpr float kBank1FbAtten = 0.65f;
    static constexpr float kBank2FbAtten = 0.75f;
};
