/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HiFlyerAudioProcessorEditor::HiFlyerAudioProcessorEditor (HiFlyerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    backgroundPNG = juce::Drawable::createFromImageData(BinaryData::background_png, BinaryData::background_pngSize);
    
    setLookAndFeel(&customLNF);
    
    // Faders
    addSlider("top boost", juce::Slider::LinearVertical, false, "left", 40, 297, 60, 208);
    addSlider("sub octave", juce::Slider::LinearVertical, false, "left", 146, 297, 60, 208);
    addSlider("ring mod", juce::Slider::LinearVertical, false, "left", 252, 297, 60, 208);
    addSlider("fuzz level", juce::Slider::LinearVertical, false, "left", 358, 297, 60, 208);
    addSlider("mix", juce::Slider::LinearVertical, false, "center", 464, 297, 60, 208);
    addSlider("mod speed", juce::Slider::LinearVertical, false, "right", 570, 297, 60, 208);
    addSlider("mod ramp", juce::Slider::LinearVertical, false, "right", 676, 297, 60, 208);
    addSlider("mod depth", juce::Slider::LinearVertical, false, "right", 782, 297, 60, 208);
    addSlider("freq shift", juce::Slider::LinearVertical, false, "right", 888, 297, 60, 208);
    
    // Knobs
    addKnob("fall time", juce::Slider::RotaryHorizontalVerticalDrag, false, 217, 98, 100, 100);
    addKnob("rise time", juce::Slider::RotaryHorizontalVerticalDrag, false, 336, 98, 100, 100);
    addKnob("control mod", juce::Slider::RotaryHorizontalVerticalDrag, true, 603, 98, 100, 100);
    addKnob("treatment", juce::Slider::RotaryHorizontalVerticalDrag, true, 825, 98, 100, 100);
    addKnob("gain", juce::Slider::RotaryHorizontalVerticalDrag, false, 448, 515, 100, 100);
    
    // Switches
    addSlider("lp top boost", juce::Slider::LinearVertical, true, "left", 42, 548, 60, 67);
    addSlider("lp sub octave", juce::Slider::LinearVertical, true, "left", 146, 548, 60, 67);
    addSlider("lp ring mod", juce::Slider::LinearVertical, true, "left", 251, 548, 60, 67);
    addSlider("lp fuzz level", juce::Slider::LinearVertical, true, "left", 358, 548, 60, 67);
    addSlider("rp mod speed", juce::Slider::LinearVertical, true, "right", 569, 548, 60, 67);
    addSlider("rp mod ramp", juce::Slider::LinearVertical, true, "right", 674, 548, 60, 67);
    addSlider("rp mod depth", juce::Slider::LinearVertical, true, "right", 780, 548, 60, 67);
    addSlider("rp freq shift", juce::Slider::LinearVertical, true, "right", 886, 548, 60, 67);
    
    addSlider("trigger sens", juce::Slider::LinearVertical, true, "left", 42, 89, 60, 67);
    addSlider("buzz", juce::Slider::LinearVertical, true, "left", 146, 89, 60, 67);
    addSlider("growl", juce::Slider::LinearVertical, true, "right", 730, 86, 60, 67);
    
    setSize(1000, 658);
}

HiFlyerAudioProcessorEditor::~HiFlyerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void HiFlyerAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (backgroundPNG)
        backgroundPNG->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);
}

void HiFlyerAudioProcessorEditor::resized()
{
}
