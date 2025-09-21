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
    DBG("=== PluginEditor Constructor START ===");
    DBG("Checking APVTS pointer: " + juce::String(audioProcessor.apvts ? "VALID" : "NULL"));
    
    if (!audioProcessor.apvts)
    {
        DBG("CRITICAL ERROR: APVTS is null in editor constructor!");
        return; // Early exit to prevent crash
    }
    
    DBG("About to set window size: " + juce::String(windowWidth) + "x" + juce::String(windowHeight));
    // Set the new window size
    setSize (windowWidth, windowHeight);
    DBG("Window size set successfully");
    
    DBG("Setting up title label...");
    // Setup main title
    titleLabel.setText("Outset-Verb Multi-Effect", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    DBG("About to add title label to component...");
    addAndMakeVisible(titleLabel);
    DBG("Title label added successfully");
    
    DBG("About to setup effect containers...");
    // Setup all effect containers
    setupEffectContainers();
    
    DBG("=== PluginEditor Constructor END ===");
}

OutsetVerbAudioProcessorEditor::~OutsetVerbAudioProcessorEditor()
{
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::setupEffectContainers()
{
    DBG("=== setupEffectContainers START ===");
    DBG("APVTS pointer check: " + juce::String(audioProcessor.apvts ? "VALID" : "NULL"));
    
    if (!audioProcessor.apvts)
    {
        DBG("ERROR: Cannot setup containers - APVTS is null!");
        return;
    }
    
    try 
    {
        DBG("Creating BitCrusher container...");
        // Create BitCrusher container
        bitCrusherContainer = std::make_unique<EffectContainer>("Bit Crusher");
        bitCrusherContainer->addSlider("bitDepth", "Bit Depth", *audioProcessor.apvts);
        bitCrusherContainer->addSlider("sampleRateReduction", "Rate Reduction", *audioProcessor.apvts);
        bitCrusherContainer->addSlider("bitCrusherMix", "Mix", *audioProcessor.apvts);
        addAndMakeVisible(*bitCrusherContainer);
        DBG("BitCrusher container created successfully");
        
        DBG("Creating Delay container...");
        // Create Delay container
        delayContainer = std::make_unique<EffectContainer>("Delay");
        delayContainer->addSlider("delayTime", "Time (ms)", *audioProcessor.apvts);
        delayContainer->addSlider("delayFeedback", "Feedback", *audioProcessor.apvts);
        delayContainer->addSlider("delayMix", "Mix", *audioProcessor.apvts);
        delayContainer->addSlider("delayLowPassCutoff", "LP Cutoff", *audioProcessor.apvts);
        addAndMakeVisible(*delayContainer);
        DBG("Delay container created successfully");
        
        DBG("Creating EQ container...");
        // Create EQ container with 2-column layout for better space utilization
        eqContainer = std::make_unique<EffectContainer>("Three Band EQ", EffectContainer::LayoutMode::TwoColumn);
        eqContainer->addSlider("lowGain", "Low Gain", *audioProcessor.apvts);
        eqContainer->addSlider("lowFreq", "Low Freq", *audioProcessor.apvts);
        eqContainer->addSlider("midGain", "Mid Gain", *audioProcessor.apvts);
        eqContainer->addSlider("midFreq", "Mid Freq", *audioProcessor.apvts);
        eqContainer->addSlider("midQ", "Mid Q", *audioProcessor.apvts);
        eqContainer->addSlider("highGain", "High Gain", *audioProcessor.apvts);
        eqContainer->addSlider("highFreq", "High Freq", *audioProcessor.apvts);
        addAndMakeVisible(*eqContainer);
        DBG("EQ container created successfully with 2-column layout");
        
        DBG("Creating Reverb container...");
        // Create Reverb container
        reverbContainer = std::make_unique<EffectContainer>("Reverb");
        reverbContainer->addSlider("roomSize", "Room Size", *audioProcessor.apvts);
        reverbContainer->addSlider("damping", "Damping", *audioProcessor.apvts);
        reverbContainer->addSlider("reverbMix", "Mix", *audioProcessor.apvts);
        reverbContainer->addSlider("width", "Width", *audioProcessor.apvts);
        reverbContainer->addToggleButton("freezeMode", "Freeze", *audioProcessor.apvts);
        addAndMakeVisible(*reverbContainer);
        DBG("Reverb container created successfully");
        
        DBG("=== setupEffectContainers completed successfully ===");
        
        DBG("Triggering layout after container creation...");
        resized();
        DBG("Layout completed - UI should now be visible");
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in setupEffectContainers: " + juce::String(e.what()));
        throw;
    }
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background with dark gradient
    juce::ColourGradient gradient(juce::Colours::darkslategrey, 0, 0,
                                  juce::Colours::black, 0, static_cast<float>(getHeight()), false);
    g.setGradientFill(gradient);
    g.fillAll();
}

void OutsetVerbAudioProcessorEditor::resized()
{
    DBG("=== OutsetVerbAudioProcessorEditor::resized() called ===");
    DBG("Window size: " + juce::String(getWidth()) + "x" + juce::String(getHeight()));
    
    // Safety check - don't layout if containers aren't created yet
    if (!bitCrusherContainer || !delayContainer || !eqContainer || !reverbContainer)
    {
        DBG("WARNING: resized() called before containers are created - skipping layout");
        return;
    }
    
    try 
    {
        auto bounds = getLocalBounds();
        DBG("Local bounds: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight()));
        
        // Position title at the top
        titleLabel.setBounds(bounds.removeFromTop(titleHeight));
        DBG("Title label positioned");
        
        // Add some padding
        bounds.reduce(containerPadding, containerPadding);
        DBG("Padding applied, remaining bounds: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight()));
        
        // Calculate container width (4 equal containers)
        int containerWidth = (bounds.getWidth() - (containerPadding * 3)) / 4;
        DBG("Container width calculated: " + juce::String(containerWidth));
        
        // Position each container horizontally
        DBG("Positioning BitCrusher container...");
        bitCrusherContainer->setBounds(bounds.removeFromLeft(containerWidth));
        bounds.removeFromLeft(containerPadding); // spacing
        
        DBG("Positioning Delay container...");
        delayContainer->setBounds(bounds.removeFromLeft(containerWidth));
        bounds.removeFromLeft(containerPadding); // spacing
        
        DBG("Positioning EQ container...");
        eqContainer->setBounds(bounds.removeFromLeft(containerWidth));
        bounds.removeFromLeft(containerPadding); // spacing
        
        DBG("Positioning Reverb container...");
        reverbContainer->setBounds(bounds); // remaining space
        
        DBG("=== resized() completed successfully ===");
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in resized(): " + juce::String(e.what()));
        throw;
    }
}
