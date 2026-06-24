/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"

//==============================================================================
/**
*/
class HiFlyerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HiFlyerAudioProcessorEditor (HiFlyerAudioProcessor&);
    ~HiFlyerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void addSlider (const juce::String& paramID,
                    juce::Slider::SliderStyle style,
                    bool isSwitch,
                    const juce::String& position,
                    int x,
                    int y,
                    int width,
                    int height);

    void addKnob (const juce::String& paramID,
                  juce::Slider::SliderStyle style,
                  bool isNotched,
                  int x,
                  int y,
                  int width,
                  int height);
    
    HiFlyerAudioProcessor& audioProcessor;
    
    CustomLookAndFeel customLNF;
    
    std::map<juce::String, std::unique_ptr<juce::Slider>> sliders;
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    
    std::unique_ptr<juce::Drawable> backgroundPNG;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HiFlyerAudioProcessorEditor)
};
