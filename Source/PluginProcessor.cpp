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
            *this, nullptr, "Parameters", createParameterLayout());
        
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

void OutsetVerbAudioProcessor::updateChainParameters()
{
    // Update BitCrusher parameters
    auto& bitCrusherNode = processorChain.get<bitCrusherIndex>();
    bitCrusherNode.setBitDepth(apvts->getRawParameterValue("bitDepth")->load());
    bitCrusherNode.setSampleRateReduction(apvts->getRawParameterValue("sampleRateReduction")->load());
    bitCrusherNode.setMix(apvts->getRawParameterValue("bitCrusherMix")->load());
    
    // Update Delay parameters
    auto& delayNode = processorChain.get<delayIndex>();
    delayNode.setDelayTime(apvts->getRawParameterValue("delayTime")->load());
    delayNode.setFeedback(apvts->getRawParameterValue("delayFeedback")->load());
    delayNode.setMix(apvts->getRawParameterValue("delayMix")->load());
    delayNode.setLowPassCutoff(apvts->getRawParameterValue("delayLowPassCutoff")->load());
    
    // Update EQ parameters
    auto& eqNode = processorChain.get<eqIndex>();
    eqNode.setLowGain(apvts->getRawParameterValue("lowGain")->load());
    eqNode.setLowFreq(apvts->getRawParameterValue("lowFreq")->load());
    eqNode.setMidGain(apvts->getRawParameterValue("midGain")->load());
    eqNode.setMidFreq(apvts->getRawParameterValue("midFreq")->load());
    eqNode.setMidQ(apvts->getRawParameterValue("midQ")->load());
    eqNode.setHighGain(apvts->getRawParameterValue("highGain")->load());
    eqNode.setHighFreq(apvts->getRawParameterValue("highFreq")->load());
    
    // Update Reverb parameters
    auto& reverbNode = processorChain.get<reverbIndex>();
    reverbNode.setRoomSize(apvts->getRawParameterValue("roomSize")->load());
    reverbNode.setDamping(apvts->getRawParameterValue("damping")->load());
    reverbNode.setWidth(apvts->getRawParameterValue("width")->load());
    
    // Handle freeze mode - convert bool to float
    bool freezeMode = apvts->getRawParameterValue("freezeMode")->load() > 0.5f;
    reverbNode.setFreezeMode(freezeMode ? 1.0f : 0.0f);
    
    // Handle reverb mix parameter
    float reverbMixValue = apvts->getRawParameterValue("reverbMix")->load();
    reverbNode.setMix(reverbMixValue);
}


void OutsetVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Prepare the processor chain with the current audio specs
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    
    processorChain.prepare(spec);
    
    // Update reverb parameters to current APVTS values
    updateChainParameters();
    
    // Optional: Log initialization for debugging
    DBG("ProcessorChain initialized - Sample Rate: " + juce::String(sampleRate) + 
        ", Buffer Size: " + juce::String(samplesPerBlock) +
        ", Channels: " + juce::String(getTotalNumOutputChannels()));
}


void OutsetVerbAudioProcessor::releaseResources()
{
    // Reset the entire processor chain
    processorChain.reset();
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

    // Update reverb parameters from APVTS
    updateChainParameters();

    // Create audio block from buffer for DSP processing
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    
    // Create process context
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    
    // Process through the entire chain
    processorChain.process(context);
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

juce::AudioProcessorValueTreeState::ParameterLayout OutsetVerbAudioProcessor::createParameterLayout()
{
    DBG("=== createParameterLayout START ===");
    
    try 
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        DBG("Adding BitCrusher parameters...");
        // BitCrusher parameters
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("bitDepth", 1),
            "Bit Depth",
            juce::NormalisableRange<float>(1.0f, 16.0f, 1.0f),
            16.0f)
        );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("sampleRateReduction", 1),
        "Sample Rate Reduction",
        juce::NormalisableRange<float>(1.0f, 50.0f, 1.0f),
        1.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bitCrusherMix", 1),
        "BitCrusher Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f)
    );

    // Delay parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayTime", 1),
        "Delay Time",
        juce::NormalisableRange<float>(0.0f, 2000.0f, 1.0f),
        250.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayFeedback", 1),
        "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f),
        0.3f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayMix", 1),
        "Delay Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayLowPassCutoff", 1),
        "Delay Low Pass",
        juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f),
        8000.0f)
    );

    // EQ parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowGain", 1),
        "Low Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowFreq", 1),
        "Low Freq",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f),
        200.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midGain", 1),
        "Mid Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midFreq", 1),
        "Mid Freq",
        juce::NormalisableRange<float>(200.0f, 5000.0f, 1.0f),
        1000.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midQ", 1),
        "Mid Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f),
        1.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("highGain", 1),
        "High Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("highFreq", 1),
        "High Freq",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f),
        8000.0f)
    );

    // Reverb parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("roomSize", 1),
        "Room Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("damping", 1),
        "Dampening",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbMix", 1),
        "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f)
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("width", 1),
        "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f)
    );

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("freezeMode", 1),
        "Freeze",
        false)
    );

        DBG("createParameterLayout completed successfully");
        return layout;
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in createParameterLayout: " + juce::String(e.what()));
        throw;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OutsetVerbAudioProcessor();
}
