/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "OutsetVerbUI.h"

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
    // UI component
    std::unique_ptr<OutsetVerbUI> verbUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutsetVerbAudioProcessorEditor)
};
