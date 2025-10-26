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
class OutsetVerbAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private juce::AudioProcessorValueTreeState::Listener
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

    // Chain ordering UI components
    std::array<std::unique_ptr<juce::ComboBox>, 4> chainDropdowns;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 4> chainAttachments;
    juce::Label audioInputLabel, audioOutputLabel;
    std::array<juce::Label, 3> flowArrows;

    // Main title label
    juce::Label titleLabel;
    
    // Layout constants
    static constexpr int windowWidth = 950;  // Increased by 150px for better spacing
    static constexpr int windowHeight = 700; // Unchanged
    static constexpr int titleHeight = 40;   // Unchanged
    static constexpr int chainOrderingHeight = 60;  // Increased for better proportions
    static constexpr int containerPadding = 12;     // Slightly increased for better spacing
    
    /** Initializes all effect containers with their parameters. */
    void setupEffectContainers();

    /** Initializes the chain ordering UI components. */
    void setupChainOrderingUI();

    /** Updates EffectContainer enabled states based on current chain configuration. */
    void updateEffectContainerStates();

    /** Updates the available options in chain dropdowns based on current selections. */
    void updateChainDropdownOptions();

    // AudioProcessorValueTreeState::Listener override
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutsetVerbAudioProcessorEditor)
};
