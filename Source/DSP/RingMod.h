/*
  =============================================================================

    RingMod.
    Created: 24 Mar 2026 11:01:59a
    Author:  Marcello Cirell

  =============================================================================
*/

#pragma onc
#include <juce_dsp/juce_dsp.h>

class RingMod
{
    public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    float processSample (float inpur, float amount) noexcept;
    private:
    // Differential pair knee width — thermal voltage scaled to audio levels
    static constexpr float kSoftness = 0.01f;
};
