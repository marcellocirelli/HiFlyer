/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 7 Feb 2026 11:14:33am
    Author:  Marcello Cirelli

  ==============================================================================
*/

#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    knobFilmstrip = juce::ImageCache::getFromMemory(BinaryData::knob128x128_png, BinaryData::knob128x128_pngSize);
    notchedKnobFilmstrip = juce::ImageCache::getFromMemory(BinaryData::notchedKnob128x128_png, BinaryData::notchedKnob128x128_pngSize);
    faderLeftSVG = juce::Drawable::createFromImageData(BinaryData::fader_left_svg, BinaryData::fader_left_svgSize);
    faderRightSVG = juce::Drawable::createFromImageData(BinaryData::fader_right_svg, BinaryData::fader_right_svgSize);
    faderCenterSVG = juce::Drawable::createFromImageData(BinaryData::fader_center_svg, BinaryData::fader_center_svgSize);
    switchLeftSVG = juce::Drawable::createFromImageData(BinaryData::switch_left_svg, BinaryData::switch_left_svgSize);
    switchRightSVG = juce::Drawable::createFromImageData(BinaryData::switch_right_svg, BinaryData::switch_right_svgSize);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    const bool isNotched = slider.getProperties()["isNotched"];
    
    
    if (!isNotched)
    {
        const int frameId = (int)std::floor(sliderPos * ((double)knobNumFrames - 1.0));
        
        const int imgWidth = knobFilmstrip.getWidth();
        const int imgHeight = knobFilmstrip.getHeight() / knobNumFrames;
        
        g.drawImage(knobFilmstrip,
                   x, y, width, height,
                   0, frameId * imgHeight,
                   imgWidth, imgHeight);
    }
    else
    {
        const int frameId = (int)std::floor(sliderPos * ((double)knobNumFrames - 1.0));
        
        const int imgWidth = notchedKnobFilmstrip.getWidth();
        const int imgHeight = notchedKnobFilmstrip.getHeight() / knobNumFrames;
        
        g.drawImage(notchedKnobFilmstrip,
                   x, y, width, height,
                   0, frameId * imgHeight,
                   imgWidth, imgHeight);
    }
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle, juce::Slider &slider)
{
    const bool isSwitch = slider.getProperties()["isSwitch"];
    const juce::String position = slider.getProperties()["position"].toString();

    juce::Drawable* svg = nullptr;

    int defaultThumbWidth  = isSwitch ? 20 : 30;
    int defaultThumbHeight = isSwitch ? 20 : 30;

    if (isSwitch)
    {
        svg = (position == "left") ? switchLeftSVG.get()
                                   : switchRightSVG.get();
    }
    else
    {
        if (position == "left") {
            svg = faderLeftSVG.get();
        } else if (position == "right") {
            svg = faderRightSVG.get();
        } else {
            svg = faderCenterSVG.get();
        }
    }

    if (!svg)
        return;

    const float thumbWidth =
        slider.getProperties().getWithDefault("thumbWidth",  defaultThumbWidth);
    const float thumbHeight =
        slider.getProperties().getWithDefault("thumbHeight", defaultThumbHeight);

    const float halfThumbH = thumbHeight * 0.5f;

    const float proportion =
        (float) slider.valueToProportionOfLength(slider.getValue());

    const float top    = y + halfThumbH;
    const float bottom = y + height - halfThumbH;

    const float thumbCentreY =
        juce::jmap(proportion, bottom, top);

    const float centreX = x + width * 0.5f;

    juce::Rectangle<float> thumbBounds(
        centreX - thumbWidth  * 0.5f,
        thumbCentreY - halfThumbH,
        thumbWidth,
        thumbHeight
    );

    svg->drawWithin(g,
                    thumbBounds,
                    juce::RectanglePlacement::centred,
                    1.0f);
}
