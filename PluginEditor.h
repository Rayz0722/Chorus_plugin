/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Mu45ChorusAudioProcessorEditor  : public juce::AudioProcessorEditor,public juce::Slider::Listener, public juce::Button::Listener
{
public:
    Mu45ChorusAudioProcessorEditor (Mu45ChorusAudioProcessor&);
    ~Mu45ChorusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    float randomGenerator(float min, float max);
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Mu45ChorusAudioProcessor& audioProcessor;
    juce::Slider delaySlider;
    juce::Slider rateSlider;
    //juce::Slider phaseSlider;
    juce::Slider fbSlider;
    juce::Slider mixSlider;
    juce::Slider depthSlider;
    juce::TextButton randomButton {"Random"};
    

    juce::Label delayLabel;
    juce::Label rateLabel;
    //juce::Label phaseLabel;
    juce::Label fbLabel;
    juce::Label mixLabel;
    juce::Label depthLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mu45ChorusAudioProcessorEditor)
};
