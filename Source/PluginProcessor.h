#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "Effects/ReverbNode.h"
#include "Effects/BitCrusherNode.h"
#include "Effects/DelayNode.h"
#include "Effects/ThreeBandEQNode.h"

//==============================================================================
/**
*/
class OutsetVerbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    OutsetVerbAudioProcessor();
    ~OutsetVerbAudioProcessor() override;

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
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    // Declare as a unique_ptr so we can initialize it later
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;
private:
    //==============================================================================
    // Individual effect processors for dynamic chain
    BitCrusherNode bitCrusherProcessor;
    DelayNode delayProcessor;
    ThreeBandEQNode eqProcessor;
    ReverbNode reverbProcessor;

    // Chain configuration - stores which effect is in each position (0 = None)
    enum EffectType
    {
        none = 0,
        bitCrusher = 1,
        delay = 2,
        eq = 3,
        reverb = 4
    };

    std::array<int, 4> chainConfiguration = {0, 0, 0, 0}; // Default: all None

    // Helper method to update reverb parameters from APVTS
    void updateChainParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutsetVerbAudioProcessor)
};
