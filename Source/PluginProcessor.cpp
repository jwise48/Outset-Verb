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
    DBG("=== PluginProcessor Constructor START ===");
    DBG("Constructor: AudioProcessor base class initialized");
    
    DBG("About to create APVTS...");
    try 
    {
        // Now initialize APVTS after base class is fully constructed
        apvts = std::make_unique<juce::AudioProcessorValueTreeState>(
            *this, nullptr, "Parameters", OutsetVerbEngine::createParameterLayout());
        
        if (apvts)
            DBG("APVTS created successfully - pointer is valid");
        else
            DBG("ERROR: APVTS is null after creation!");
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION during APVTS creation: " + juce::String(e.what()));
        throw;
    }
    
    DBG("About to create engine...");
    // Create the audio processing engine
    engine = std::make_unique<OutsetVerbEngine>(*apvts);
    DBG("Engine created successfully");
    
    DBG("=== PluginProcessor Constructor END ===");
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

void OutsetVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Prepare the audio processing engine
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    if (engine)
        engine->prepare(spec);

    DBG("Engine prepared - Sample Rate: " + juce::String(sampleRate) +
        ", Buffer Size: " + juce::String(samplesPerBlock) +
        ", Channels: " + juce::String(getTotalNumOutputChannels()));
}


void OutsetVerbAudioProcessor::releaseResources()
{
    // Reset the audio processing engine
    if (engine)
        engine->reset();
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

    // Process through the audio engine
    if (engine)
        engine->processBlock(buffer);
}

//==============================================================================
bool OutsetVerbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OutsetVerbAudioProcessor::createEditor()
{
    // Return our custom editor with organized effect containers
    return new OutsetVerbAudioProcessorEditor (*this);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OutsetVerbAudioProcessor();
}
