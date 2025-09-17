/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OutsetVerbAudioProcessor::OutsetVerbAudioProcessor()
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
    DBG("Constructor: AudioProcessor base class initialized");
    
    // Now initialize APVTS after base class is fully constructed
    apvts = std::make_unique<juce::AudioProcessorValueTreeState>(
        *this, nullptr, "Parameters", createParameterLayout());
    
    DBG("Constructor: APVTS initialized successfully");
    DBG("Constructor: Parameter count: " + juce::String(getParameters().size()));
}

OutsetVerbAudioProcessor::~OutsetVerbAudioProcessor()
{
}

//==============================================================================
const juce::String OutsetVerbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OutsetVerbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OutsetVerbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OutsetVerbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OutsetVerbAudioProcessor::getTailLengthSeconds() const
{
    return 3.0;
}

int OutsetVerbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OutsetVerbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OutsetVerbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OutsetVerbAudioProcessor::getProgramName (int index)
{
    return {};
}

void OutsetVerbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void OutsetVerbAudioProcessor::updateReverbParameters()
{
    // Create a new parameters struct
    juce::Reverb::Parameters params;
    
    // Get values from APVTS and map them to reverb parameters
    params.roomSize = apvts->getRawParameterValue("roomSize")->load();
    params.damping = apvts->getRawParameterValue("damping")->load();
    params.width = apvts->getRawParameterValue("width")->load();
    
    // Handle freeze mode - convert bool to float
    bool freezeMode = apvts->getRawParameterValue("freezeMode")->load() > 0.5f;
    params.freezeMode = freezeMode ? 1.0f : 0.0f;
    
    // Handle mix parameter by setting wet/dry levels
    float mixValue = apvts->getRawParameterValue("mix")->load();
    params.wetLevel = mixValue;           // Mix = 1.0 means full wet
    params.dryLevel = 1.0f - mixValue;    // Mix = 0.0 means full dry
    
    // Apply the parameters to the reverb
    reverb.setParameters(params);
}
void OutsetVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize the reverb with the current sample rate
    reverb.setSampleRate(sampleRate);
    
    // Clear any existing reverb buffers/state
    reverb.reset();
    
    // Update reverb parameters to current APVTS values
    updateReverbParameters();
    
    // Optional: Log initialization for debugging
    DBG("Reverb initialized - Sample Rate: " + juce::String(sampleRate) + 
        ", Buffer Size: " + juce::String(samplesPerBlock));
}

void OutsetVerbAudioProcessor::releaseResources()
{
    // Clear the reverb's internal buffers and reset its state
    reverb.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OutsetVerbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OutsetVerbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    // Early exit if no input channels
    if (totalNumInputChannels == 0)
        return;

    // Update reverb parameters from APVTS (this handles wet/dry internally)
    updateReverbParameters();

    // Process based on channel configuration
    if (totalNumInputChannels == 1)
    {
        // MONO PROCESSING
        auto* monoData = buffer.getWritePointer(0);
        reverb.processMono(monoData, numSamples);
    }
    else if (totalNumInputChannels >= 2)
    {
        // STEREO PROCESSING
        auto* leftData = buffer.getWritePointer(0);
        auto* rightData = buffer.getWritePointer(1);
        reverb.processStereo(leftData, rightData, numSamples);
    }

    // Handle any additional output channels (copy from stereo if needed)
    for (int channel = 2; channel < totalNumOutputChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, buffer, channel % 2, 0, numSamples);
    }
}

//==============================================================================
bool OutsetVerbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OutsetVerbAudioProcessor::createEditor()
{
    // Note: Opting to use the generic audio editor for now.
    // Uncomment line below and remove the GenericAudioProcessorEditor to customize the GUI.
    // return new OutsetVerbAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void OutsetVerbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Store the current state of the parameter tree
    
}

void OutsetVerbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore the state of the parameter tree
}

juce::AudioProcessorValueTreeState::ParameterLayout OutsetVerbAudioProcessor::createParameterLayout()
{
    DBG("createParameterLayout: Creating layout with one simple parameter");
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("roomSize", 1),  // Proper ParameterID with version hint
        "Room Size",
        juce::NormalisableRange<float>(0.f, 1.f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("damping", 1),  // Proper ParameterID with version hint
        "Dampening",
        juce::NormalisableRange<float>(0.f, 1.f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 1),  // Proper ParameterID with version hint
        "Mix",
        juce::NormalisableRange<float>(0.f, 1.f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("width", 1),  // Proper ParameterID with version hint
        "Width",
        juce::NormalisableRange<float>(0.f, 1.f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("freezeMode", 1),  // Proper ParameterID with version hint
        "Freeze",
        false)
    );


    DBG("createParameterLayout: Simple parameter added");
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OutsetVerbAudioProcessor();
}
