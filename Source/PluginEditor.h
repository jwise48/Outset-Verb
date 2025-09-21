/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "EffectContainer.h"
#include <memory>

//==============================================================================
/**
    Custom audio processor editor for the Outset-Verb multi-effect plugin.
    
    Features four effect containers arranged horizontally:
    - BitCrusher, Delay, Three Band EQ, and Reverb
*/
class OutsetVerbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OutsetVerbAudioProcessorEditor (OutsetVerbAudioProcessor&);
    ~OutsetVerbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OutsetVerbAudioProcessor& audioProcessor;
    
    //==============================================================================
    // Effect containers
    std::unique_ptr<EffectContainer> bitCrusherContainer;
    std::unique_ptr<EffectContainer> delayContainer;
    std::unique_ptr<EffectContainer> eqContainer;
    std::unique_ptr<EffectContainer> reverbContainer;
    
    // Main title label
    juce::Label titleLabel;
    
    // Layout constants
    static constexpr int windowWidth = 800;
    static constexpr int windowHeight = 650;
    static constexpr int titleHeight = 40;
    static constexpr int containerPadding = 10;
    
    /** Initializes all effect containers with their parameters. */
    void setupEffectContainers();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutsetVerbAudioProcessorEditor)
};
