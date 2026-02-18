/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 6 Feb 2026 6:15:50pm
    Author:  Marcello Cirelli

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
    public:
    CustomLookAndFeel();
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override;
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle,
                           juce::Slider& slider) override;
    
    private:
    juce::Image knobFilmstrip;
    int knobNumFrames = 101;
    juce::Image notchedKnobFilmstrip;
    std::unique_ptr<juce::Drawable> faderLeftSVG;
    std::unique_ptr<juce::Drawable> faderRightSVG;
    std::unique_ptr<juce::Drawable> faderCenterSVG;
    std::unique_ptr<juce::Drawable> switchLeftSVG;
    std::unique_ptr<juce::Drawable> switchRightSVG;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
