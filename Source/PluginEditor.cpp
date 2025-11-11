/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OutsetVerbAudioProcessorEditor::OutsetVerbAudioProcessorEditor (OutsetVerbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Create the OutsetVerbUI component
    verbUI = std::make_unique<OutsetVerbUI>(*audioProcessor.apvts);
    addAndMakeVisible(*verbUI);
    
    // Set window size to recommended size from OutsetVerbUI
    auto recommendedSize = OutsetVerbUI::getRecommendedSize();
    setSize(recommendedSize.getWidth(), recommendedSize.getHeight());
}

OutsetVerbAudioProcessorEditor::~OutsetVerbAudioProcessorEditor()
{
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background is handled by verbUI
}

void OutsetVerbAudioProcessorEditor::resized()
{
    // Simply set verbUI to fill the entire editor bounds
    if (verbUI)
        verbUI->setBounds(getLocalBounds());
}
