/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mu45ChorusAudioProcessorEditor::Mu45ChorusAudioProcessorEditor (Mu45ChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (750, 260);
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    delaySlider.setBounds(75,100,100,100);
    delaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    delaySlider.setRange(audioParam->range.start, audioParam->range.end);
    delaySlider.setValue(*audioParam);
    delaySlider.setNumDecimalPlacesToDisplay(2);
    delaySlider.setTextValueSuffix ("ms");
    delaySlider.setDoubleClickReturnValue(true, 5.0f);
    delaySlider.addListener(this);
    addAndMakeVisible(delaySlider);
    
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
    rateSlider.setBounds(200,100,100,100);
    rateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    rateSlider.setRange(audioParam->range.start, audioParam->range.end);
    rateSlider.setValue(*audioParam);
    rateSlider.setNumDecimalPlacesToDisplay(2);
    rateSlider.setTextValueSuffix ("Hz");
    rateSlider.setDoubleClickReturnValue(true, 0.08f);
    rateSlider.addListener(this);
    addAndMakeVisible(rateSlider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
    fbSlider.setBounds(450, 100, 100, 100);
    fbSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    fbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    fbSlider.setRange(audioParam->range.start, audioParam->range.end);
    fbSlider.setValue(*audioParam);
    fbSlider.setNumDecimalPlacesToDisplay(2);
    fbSlider.setTextValueSuffix ("%");
    fbSlider.setDoubleClickReturnValue(true, 0.0f);
    fbSlider.addListener(this);
    addAndMakeVisible(fbSlider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
    mixSlider.setBounds(575, 100, 100, 100);
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setRange(audioParam->range.start, audioParam->range.end);
    mixSlider.setValue(*audioParam);
    mixSlider.setNumDecimalPlacesToDisplay(2);
    mixSlider.setTextValueSuffix ("%");
    mixSlider.setDoubleClickReturnValue(true, 50.0f);
    mixSlider.addListener(this);
    addAndMakeVisible(mixSlider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
    depthSlider.setBounds(325, 100, 100, 100);
    depthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    depthSlider.setRange(audioParam->range.start, audioParam->range.end);
    depthSlider.setValue(*audioParam);
    depthSlider.setNumDecimalPlacesToDisplay(2);
    depthSlider.setTextValueSuffix ("ms");
    depthSlider.setDoubleClickReturnValue(true, 5.0f);
    depthSlider.addListener(this);
    addAndMakeVisible(depthSlider);
    
    
    randomButton.setBounds(325, 220, 100, 20);
    randomButton.addListener(this);
    addAndMakeVisible(randomButton);
    
    juce::Font customFont(15.0f);
    delayLabel.setText("Delay", juce::dontSendNotification);
    delayLabel.setBounds(delaySlider.getX() + 27, delaySlider.getY() - 25, delaySlider.getWidth(), 20);
    delayLabel.setFont(customFont);
    addAndMakeVisible(delayLabel);
    
    rateLabel.setText("Rate", juce::dontSendNotification);
    rateLabel.setBounds(rateSlider.getX() + 30, rateSlider.getY() - 25, rateSlider.getWidth(), 20);
    rateLabel.setFont(customFont);
    addAndMakeVisible(rateLabel);
    
    depthLabel.setText("Depth", juce::dontSendNotification);
    depthLabel.setBounds(depthSlider.getX() +27 , depthSlider.getY() - 25, depthSlider.getWidth(), 20);
    depthLabel.setFont(customFont);
    addAndMakeVisible(depthLabel);

    fbLabel.setText("Feedback", juce::dontSendNotification);
    fbLabel.setBounds(fbSlider.getX() + 18, fbSlider.getY() - 25, fbSlider.getWidth(), 20);
    fbLabel.setFont(customFont);
    addAndMakeVisible(fbLabel);
    
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setBounds(mixSlider.getX() +32 , mixSlider.getY() - 25, mixSlider.getWidth(), 20);
    mixLabel.setFont(customFont);
    addAndMakeVisible(mixLabel);
    
    
}

void Mu45ChorusAudioProcessorEditor::sliderValueChanged(juce::Slider * slider)
{
    auto& params = processor.getParameters();
    if (slider == &delaySlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = delaySlider.getValue();
        DBG("delay slider changed");
        audioProcessor.updateDelays();
    }
    else if (slider == &rateSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = rateSlider.getValue();
    
        DBG("rate slider changed");
    }
    else if (slider == &fbSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *audioParam = fbSlider.getValue();
        DBG("feedback slider changed");
    }
    else if (slider == &mixSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
        *audioParam = mixSlider.getValue();
        DBG("mix slider changed");
    }
    else if (slider == &depthSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
        *audioParam = depthSlider.getValue();
        DBG("depth slider changed");
    }
}

float Mu45ChorusAudioProcessorEditor::randomGenerator(float min, float max) {
    float value = juce::Random::getSystemRandom().nextFloat() * (max - min) + min;
    return std::round(value * 100.0f) / 100.0f;
}

void Mu45ChorusAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &randomButton)
    {
        // Set random values to sliders
        delaySlider.setValue(randomGenerator(0.1,20));
        rateSlider.setValue(randomGenerator(0.05,20));
        fbSlider.setValue(randomGenerator(0,95));
        mixSlider.setValue(randomGenerator(0,100));
        depthSlider.setValue(randomGenerator(1,5));
    }
}

Mu45ChorusAudioProcessorEditor::~Mu45ChorusAudioProcessorEditor()
{
}

//==============================================================================
void Mu45ChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (30.0f);
    auto localbounds = getLocalBounds();
    auto toparea = localbounds.removeFromTop(60);
    g.drawFittedText ("Chorus Effect", toparea, juce::Justification::centredTop, 1);
}

void Mu45ChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
