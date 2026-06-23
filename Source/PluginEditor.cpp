/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

//==============================================================================
HiFlyerAudioProcessorEditor::HiFlyerAudioProcessorEditor (HiFlyerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    backgroundPNG = juce::Drawable::createFromImageData (BinaryData::background_png, BinaryData::background_pngSize);
    
    setLookAndFeel (&customLNF);
    
    // Faders
    addSlider (topBoostParamID.getParamID(), juce::Slider::LinearVertical, false, "left", 40, 297, 60, 208);
    addSlider (subOctaveParamID.getParamID(), juce::Slider::LinearVertical, false, "left", 146, 297, 60, 208);
    addSlider (ringModParamID.getParamID(), juce::Slider::LinearVertical, false, "left", 252, 297, 60, 208);
    addSlider (fuzzLevelParamID.getParamID(), juce::Slider::LinearVertical, false, "left", 358, 297, 60, 208);
    addSlider (mixParamID.getParamID(), juce::Slider::LinearVertical, false, "center", 464, 297, 60, 208);
    addSlider (modSpeedParamID.getParamID(), juce::Slider::LinearVertical, false, "right", 570, 297, 60, 208);
    addSlider (modRampParamID.getParamID(), juce::Slider::LinearVertical, false, "right", 676, 297, 60, 208);
    addSlider (modDepthParamID.getParamID(), juce::Slider::LinearVertical, false, "right", 782, 297, 60, 208);
    addSlider (freqShiftParamID.getParamID(), juce::Slider::LinearVertical, false, "right", 888, 297, 60, 208);
    
    // Knobs
    addKnob (fallTimeParamID.getParamID(), juce::Slider::RotaryHorizontalVerticalDrag, false, 217, 98, 100, 100);
    addKnob (riseTimeParamID.getParamID(), juce::Slider::RotaryHorizontalVerticalDrag, false, 336, 98, 100, 100);
    addKnob (controlModParamID.getParamID(), juce::Slider::RotaryHorizontalVerticalDrag, true, 603, 98, 100, 100);
    addKnob (treatmentParamID.getParamID(), juce::Slider::RotaryHorizontalVerticalDrag, true, 825, 98, 100, 100);
    addKnob (gainParamID.getParamID(), juce::Slider::RotaryHorizontalVerticalDrag, false, 448, 515, 100, 100);
    
    // Switches
    addSlider (lpTopBoostParamID.getParamID(), juce::Slider::LinearVertical, true, "left", 42, 548, 60, 67);
    addSlider (lpSubOctaveParamID.getParamID(), juce::Slider::LinearVertical, true, "left", 146, 548, 60, 67);
    addSlider (lpRingModParamID.getParamID(), juce::Slider::LinearVertical, true, "left", 251, 548, 60, 67);
    addSlider (lpFuzzLevelParamID.getParamID(), juce::Slider::LinearVertical, true, "left", 358, 548, 60, 67);
    addSlider (rpModSpeedParamID.getParamID(), juce::Slider::LinearVertical, true, "right", 569, 548, 60, 67);
    addSlider (rpModRampParamID.getParamID(), juce::Slider::LinearVertical, true, "right", 674, 548, 60, 67);
    addSlider (rpModDepthParamID.getParamID(), juce::Slider::LinearVertical, true, "right", 780, 548, 60, 67);
    addSlider (rpFreqShiftParamID.getParamID(), juce::Slider::LinearVertical, true, "right", 886, 548, 60, 67);
    
    addSlider (triggerSensParamID.getParamID(), juce::Slider::LinearVertical, true, "left", 42, 89, 60, 67);
    addSlider (buzzParamID.getParamID(), juce::Slider::LinearVertical, true, "left", 146, 89, 60, 67);
    addSlider (growlParamID.getParamID(), juce::Slider::LinearVertical, true, "right", 730, 86, 60, 67);
    
    setSize (1000, 658);
}

HiFlyerAudioProcessorEditor::~HiFlyerAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void HiFlyerAudioProcessorEditor::addSlider (const juce::String& paramID,
                                             juce::Slider::SliderStyle style,
                                             bool isSwitch,
                                             const juce::String& position,
                                             int x,
                                             int y,
                                             int width,
                                             int height)
{
    auto slider = std::make_unique<juce::Slider>();
    slider->setSliderStyle (style);
    slider->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider->getProperties().set ("isSwitch", isSwitch);
    slider->getProperties().set ("position", position);

    const auto boundsString = juce::String (x) + " " + juce::String (y) + " "
                            + juce::String (width) + " " + juce::String (height);
    slider->getProperties().set ("originalBounds", boundsString);
    slider->setBounds (x, y, width, height);

    addAndMakeVisible (*slider);

    sliderAttachments[paramID] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, paramID, *slider);

    sliders[paramID] = std::move (slider);
}

void HiFlyerAudioProcessorEditor::addKnob (const juce::String& paramID,
                                           juce::Slider::SliderStyle style,
                                           bool isNotched,
                                           int x,
                                           int y,
                                           int width,
                                           int height)
{
    auto slider = std::make_unique<juce::Slider>();
    slider->setSliderStyle (style);
    slider->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider->setBounds (x, y, width, height);
    slider->getProperties().set ("isNotched", isNotched);

    addAndMakeVisible (*slider);

    sliderAttachments[paramID] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, paramID, *slider);

    sliders[paramID] = std::move (slider);
}

//==============================================================================
void HiFlyerAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (backgroundPNG)
        backgroundPNG->drawWithin (g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);
}

void HiFlyerAudioProcessorEditor::resized()
{
}
