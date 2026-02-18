/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
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
    
    void addSlider(const juce::String& paramID, juce::Slider::SliderStyle style, bool isSwitch, const juce::String& position, int x, int y, int width, int height)
    {
        auto slider = std::make_unique<juce::Slider>();
        slider->setSliderStyle(style);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider->getProperties().set("isSwitch", isSwitch);
        slider->getProperties().set("position", position);
        
        auto boundsString = juce::String(x) + " " + juce::String(y) + " " + juce::String(width) + " " + juce::String(height);
        slider->getProperties().set("originalBounds", boundsString);
        slider->setBounds(x, y, width, height);
        
        addAndMakeVisible(*slider);
        
        sliderAttachments[paramID] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, paramID, *slider);
        
        sliders[paramID] = std::move(slider);
    }
    
    void addKnob(const juce::String& paramID, juce::Slider::SliderStyle style, bool isNotched, int x, int y, int width, int height)
    {
        auto slider = std::make_unique<juce::Slider>();
        slider->setSliderStyle(style);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider->setBounds(x, y, width, height);
        slider->getProperties().set("isNotched", isNotched);
            
        addAndMakeVisible(*slider);
        
        sliderAttachments[paramID] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, paramID, *slider);
        
        sliders[paramID] = std::move(slider);
    }
    
    HiFlyerAudioProcessor& audioProcessor;
    
    CustomLookAndFeel customLNF;
    
    std::map<juce::String, std::unique_ptr<juce::Slider>> sliders;
    std::map<juce::String, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    
    std::unique_ptr<juce::Drawable> backgroundPNG;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HiFlyerAudioProcessorEditor)
};
