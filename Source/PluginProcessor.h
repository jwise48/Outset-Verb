#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "OutsetVerbEngine.h"

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
    
    // Declare as a unique_ptr so we can initialize it later
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;
private:
    //==============================================================================
    // Audio processing engine
    std::unique_ptr<OutsetVerbEngine> engine;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutsetVerbAudioProcessor)
};
