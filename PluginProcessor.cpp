/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mu45ChorusAudioProcessor::Mu45ChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(delayParam = new juce::AudioParameterFloat("delay time (ms)", // parameterID,
        "Delay", // parameterName,
        0.1f, // minValue,
        25.0f, // maxValue,
        5.0f)); // defaultValue
    addParameter(rateParam = new juce::AudioParameterFloat("LFO rate (Hz)", // parameterID,
        "lforate", // parameterName,
        0.0f, // minValue,
        20.0f, // maxValue,
        0.08f)); // defaultValue
    addParameter(fbParam = new juce::AudioParameterFloat("feedback gain (%)", // parameterID,
        "feedback", // parameterName,
        0.0f, // minValue,
        75.0f, // maxValue,
        0.0f)); // defaultValue
    addParameter(mixParam = new juce::AudioParameterFloat("mix percentage (%)", // parameterID,
        "Mix", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        50.0f)); // defaultValue
    addParameter(depthParam = new juce::AudioParameterFloat("depth (ms)", // parameterID,
        "depth", // parameterName,
        0.0f, // minValue,
        5.0f, // maxValue,
        5.0f)); // defaultValue
    
}

Mu45ChorusAudioProcessor::~Mu45ChorusAudioProcessor()
{
}

//==============================================================================
const juce::String Mu45ChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Mu45ChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Mu45ChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Mu45ChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Mu45ChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Mu45ChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Mu45ChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Mu45ChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Mu45ChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void Mu45ChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

int Mu45ChorusAudioProcessor::calcDelaySampsFromMsec(float msec)
{
    return static_cast<int>(msec * mFs / 1000.0f);
}

// set the delays from the parameter
void Mu45ChorusAudioProcessor::calcDelays()
{
    float msecL = delayParam->get()+ (lfoOutL * depthParam->get()); //add  depth
    int sampsL = calcDelaySampsFromMsec(msecL);
    
    float msecR = delayParam->get() + (lfoOutL * depthParam->get()); //add  depth
    int sampsR = calcDelaySampsFromMsec(msecR);
    
    delay1L.setDelay(sampsL);
    delay1R.setDelay(sampsR);
    
}

//==============================================================================
void Mu45ChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mFs = sampleRate;
    
    int samps = calcDelaySampsFromMsec(maxDelayMsec);
    delay1L.setMaximumDelay(samps);
    delay1R.setMaximumDelay(samps);
    //calcDelays();
    LFOR.setPhase(90.0);
}

void Mu45ChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Mu45ChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Mu45ChorusAudioProcessor::calcAlgorithmParams()
{
    LFOL.setFreq(rateParam->get(), mFs);
    LFOR.setFreq(rateParam->get(), mFs);
    
}
void Mu45ChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    calcAlgorithmParams();
    float dryGain = 1.0 - 1.0 *mixParam->get()/100.0;
    float wetGain = 1.0 * mixParam->get()/100.0;
    float fb = fbParam->get()/100.0;
    float depth = depthParam->get();
    
    // This version assumes we always have 2 channels
    float* channelDataL = buffer.getWritePointer (0);
    float* channelDataR = buffer.getWritePointer (1);
    float tempL, tempR, temp2L, temp2R;
    //float filterL,filterR;
    // The "inner loop"
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        lfoOutL = LFOL.tick();
        lfoOutR = LFOR.tick();
        
        // Calculate the new modulated delay time based on the base delay and LFO modulation
        float newDelayL = delayParam->get() + lfoOutL*depth;
        float newDelayR = delayParam->get() + lfoOutR*depth;
        
        // Convert ms to samples for the actual delay line
        int delaySamplesL = calcDelaySampsFromMsec(newDelayL);
        int delaySamplesR = calcDelaySampsFromMsec(newDelayR);

        // Set the new delay times
        delay1L.setDelay(delaySamplesL);
        delay1R.setDelay(delaySamplesR);
        
        tempL = delay1L.nextOut();
        tempR = delay1R.nextOut();
        
        temp2L = tempL * fb;
        temp2R = tempR * fb;

        delay1L.tick(channelDataL[i] + temp2L);
        delay1R.tick(channelDataR[i]+ temp2R);
        
        channelDataL[i] = dryGain *channelDataL[i] + wetGain * tempL;
        channelDataR[i] = dryGain *channelDataR[i] + wetGain * tempR;
  
        
    }
}

//==============================================================================
bool Mu45ChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Mu45ChorusAudioProcessor::createEditor()
{
    return new Mu45ChorusAudioProcessorEditor (*this);
}

//==============================================================================
void Mu45ChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Mu45ChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Mu45ChorusAudioProcessor();
}
