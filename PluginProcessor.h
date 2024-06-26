/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.2/Delay.h"
#include "Mu45LFO/Mu45LFO.h"
//==============================================================================
/**
*/
class Mu45ChorusAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Mu45ChorusAudioProcessor();
    ~Mu45ChorusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateDelays()
    {
        calcDelays();
    }
    
    
private:
    //==============================================================================
    Mu45LFO LFOL, LFOR;
    
    float mFs; // Sampling rate
    const float maxDelayMsec = 1000.0f;
    //float fb; //feedback
    
    void calcAlgorithmParams();
    void calcDelays();
    int calcDelaySampsFromMsec(float time);
    
    //User parameters
    juce::AudioParameterFloat* mixParam; //wet and dry mix
    juce::AudioParameterFloat* fbParam; //feedback
    juce::AudioParameterFloat* delayParam; //delay time
    juce::AudioParameterFloat* rateParam; //rate of lfo
    juce::AudioParameterFloat* depthParam; // depth 
    
    stk::Delay delay1L, delay1R;
    
    float lfoOutL, lfoOutR;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mu45ChorusAudioProcessor)
};
