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
    
    DBG("About to setup chain ordering UI...");
    try {
        // Setup chain ordering UI first
        setupChainOrderingUI();
        DBG("Chain ordering UI setup completed successfully");
    }
    catch (const std::exception& e) {
        DBG("EXCEPTION in setupChainOrderingUI: " + juce::String(e.what()));
        DBG("Chain ordering UI setup failed - plugin will continue without it");
    }

    DBG("About to setup effect containers...");
    // Setup all effect containers
    setupEffectContainers();

    DBG("About to update chain dropdown options...");
    // Update dropdown options based on initial chain configuration
    updateChainDropdownOptions();

    DBG("About to update effect container states...");
    // Update effect container states based on initial chain configuration
    updateEffectContainerStates();

    DBG("About to add parameter listener...");
    // Add parameter listener for chain configuration changes
    if (audioProcessor.apvts)
        audioProcessor.apvts->addParameterListener("chainSlot1", this);
        audioProcessor.apvts->addParameterListener("chainSlot2", this);
        audioProcessor.apvts->addParameterListener("chainSlot3", this);
        audioProcessor.apvts->addParameterListener("chainSlot4", this);

    DBG("About to trigger initial layout...");
    // Trigger initial layout now that all components are created
    resized();
    DBG("Initial layout completed - UI should now be visible");

    DBG("=== PluginEditor Constructor END ===");
}

OutsetVerbAudioProcessorEditor::~OutsetVerbAudioProcessorEditor()
{
    // Remove parameter listeners
    if (audioProcessor.apvts)
    {
        audioProcessor.apvts->removeParameterListener("chainSlot1", this);
        audioProcessor.apvts->removeParameterListener("chainSlot2", this);
        audioProcessor.apvts->removeParameterListener("chainSlot3", this);
        audioProcessor.apvts->removeParameterListener("chainSlot4", this);
    }
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    // Update effect container states when chain configuration changes
    if (parameterID.startsWith("chainSlot"))
    {
        updateChainDropdownOptions();  // Update dropdown options first
        updateEffectContainerStates();
    }
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
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in setupEffectContainers: " + juce::String(e.what()));
        throw;
    }
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::setupChainOrderingUI()
{
    DBG("=== setupChainOrderingUI START ===");

    if (!audioProcessor.apvts)
    {
        DBG("ERROR: Cannot setup chain ordering UI - APVTS is null!");
        return;
    }

    DBG("APVTS is valid, proceeding with chain ordering UI setup");

    try
    {
        DBG("Setting up audio input label...");
        // Setup audio input label
        audioInputLabel.setText("Audio Input", juce::dontSendNotification);
        audioInputLabel.setFont(juce::Font(14.0f));
        audioInputLabel.setJustificationType(juce::Justification::centred);
        audioInputLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(audioInputLabel);

        // Setup flow arrows
        for (int i = 0; i < 3; ++i)
        {
            flowArrows[i].setText("->", juce::dontSendNotification);
            flowArrows[i].setFont(juce::Font(16.0f, juce::Font::bold));
            flowArrows[i].setJustificationType(juce::Justification::centred);
            flowArrows[i].setColour(juce::Label::textColourId, juce::Colours::lightblue);
            addAndMakeVisible(flowArrows[i]);
        }

        // Setup audio output label
        audioOutputLabel.setText("Audio Output", juce::dontSendNotification);
        audioOutputLabel.setFont(juce::Font(14.0f));
        audioOutputLabel.setJustificationType(juce::Justification::centred);
        audioOutputLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(audioOutputLabel);

        // Setup chain dropdowns and their attachments
        const juce::StringArray effectOptions = {"None", "Bit Crusher", "Delay", "EQ", "Reverb"};

        for (int i = 0; i < 4; ++i)
        {
            // Create dropdown
            chainDropdowns[i] = std::make_unique<juce::ComboBox>();
            chainDropdowns[i]->addItemList(effectOptions, 1);  // Start IDs from 1
            chainDropdowns[i]->setSelectedId(1);  // Default to "None" (ID 1)
            addAndMakeVisible(*chainDropdowns[i]);

            // Create attachment
            juce::String paramID = "chainSlot" + juce::String(i + 1);
            chainAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                *audioProcessor.apvts, paramID, *chainDropdowns[i]);
        }

        DBG("=== setupChainOrderingUI completed successfully ===");
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in setupChainOrderingUI: " + juce::String(e.what()));
        throw;
    }
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::updateEffectContainerStates()
{
    if (!audioProcessor.apvts)
        return;

    // Get current chain configuration
    std::array<int, 4> chainConfig;
    chainConfig[0] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot1")->load());
    chainConfig[1] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot2")->load());
    chainConfig[2] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot3")->load());
    chainConfig[3] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot4")->load());

    // Check which effects are currently in the chain
    bool bitCrusherInChain = false;
    bool delayInChain = false;
    bool eqInChain = false;
    bool reverbInChain = false;

    for (int config : chainConfig)
    {
        switch (config)
        {
            case 1: bitCrusherInChain = true; break;  // Bit Crusher
            case 2: delayInChain = true; break;       // Delay
            case 3: eqInChain = true; break;          // EQ
            case 4: reverbInChain = true; break;      // Reverb
            default: break;  // None or invalid
        }
    }

    // Update container states AND visibility
    if (bitCrusherContainer)
    {
        bitCrusherContainer->setEnabledState(bitCrusherInChain);
        bitCrusherContainer->setVisible(bitCrusherInChain);
    }

    if (delayContainer)
    {
        delayContainer->setEnabledState(delayInChain);
        delayContainer->setVisible(delayInChain);
    }

    if (eqContainer)
    {
        eqContainer->setEnabledState(eqInChain);
        eqContainer->setVisible(eqInChain);
    }

    if (reverbContainer)
    {
        reverbContainer->setEnabledState(reverbInChain);
        reverbContainer->setVisible(reverbInChain);
    }

    // Trigger layout update to reposition visible containers
    resized();
}

//==============================================================================
void OutsetVerbAudioProcessorEditor::updateChainDropdownOptions()
{
    if (!audioProcessor.apvts)
        return;

    // Get current chain configuration
    std::array<int, 4> chainConfig;
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramID = "chainSlot" + juce::String(i + 1);
        chainConfig[i] = static_cast<int>(audioProcessor.apvts->getRawParameterValue(paramID)->load());
    }

    // Update each dropdown
    for (int dropdownIndex = 0; dropdownIndex < 4; ++dropdownIndex)
    {
        if (!chainDropdowns[dropdownIndex])
            continue;

        // Effect IDs: 1=None, 2=BitCrusher, 3=Delay, 4=EQ, 5=Reverb
        // Check which effects are used in OTHER slots
        for (int effectID = 2; effectID <= 5; ++effectID)  // Skip "None" (ID 1)
        {
            bool isUsedInOtherSlot = false;
            
            // Check all OTHER slots for this effect
            for (int otherSlot = 0; otherSlot < 4; ++otherSlot)
            {
                if (otherSlot != dropdownIndex && chainConfig[otherSlot] + 1 == effectID)
                {
                    isUsedInOtherSlot = true;
                    break;
                }
            }
            
            // Enable/disable the item in the current dropdown
            chainDropdowns[dropdownIndex]->setItemEnabled(effectID, !isUsedInOtherSlot);
        }
        
        // "None" (ID 1) should always be enabled
        chainDropdowns[dropdownIndex]->setItemEnabled(1, true);
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

        // Only layout chain ordering UI if it was successfully created
        if (chainDropdowns[0] && chainDropdowns[1] && chainDropdowns[2] && chainDropdowns[3])
        {
            DBG("Chain ordering UI components exist - positioning them");

            // Position chain ordering UI below title
            auto chainBounds = bounds.removeFromTop(chainOrderingHeight);
            chainBounds.reduce(containerPadding, 5);

            // Improved spacing calculation with dedicated widths
            int inputLabelWidth = 80;
            int outputLabelWidth = 90;
            int arrowWidth = 25;
            int dropdownSpacing = 8;
            int totalFixedWidth = inputLabelWidth + outputLabelWidth + (3 * arrowWidth) + (7 * dropdownSpacing);
            int availableDropdownWidth = chainBounds.getWidth() - totalFixedWidth;
            int dropdownWidth = availableDropdownWidth / 4;

            // Layout with proper spacing
            audioInputLabel.setBounds(chainBounds.removeFromLeft(inputLabelWidth));
            chainBounds.removeFromLeft(dropdownSpacing);

            for (int i = 0; i < 4; ++i)
            {
                if (i > 0)
                {
                    flowArrows[i-1].setBounds(chainBounds.removeFromLeft(arrowWidth));
                    chainBounds.removeFromLeft(dropdownSpacing);
                }

                chainDropdowns[i]->setBounds(chainBounds.removeFromLeft(dropdownWidth));
                if (i < 3) chainBounds.removeFromLeft(dropdownSpacing);
            }

            chainBounds.removeFromLeft(dropdownSpacing);
            audioOutputLabel.setBounds(chainBounds.removeFromLeft(outputLabelWidth));

            DBG("Chain ordering UI positioned with improved spacing");
        }
        else
        {
            DBG("WARNING: Chain ordering UI components not created - skipping chain layout");
            bounds.removeFromTop(chainOrderingHeight);
        }

        // Add padding below chain ordering
        bounds.reduce(containerPadding, containerPadding);
        DBG("Padding applied, remaining bounds: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight()));
        
        // DYNAMIC CONTAINER POSITIONING BASED ON CHAIN CONFIGURATION
        // Get current chain configuration
        std::array<int, 4> chainConfig = {0, 0, 0, 0};
        if (audioProcessor.apvts)
        {
            chainConfig[0] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot1")->load());
            chainConfig[1] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot2")->load());
            chainConfig[2] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot3")->load());
            chainConfig[3] = static_cast<int>(audioProcessor.apvts->getRawParameterValue("chainSlot4")->load());
        }
        
        // Calculate container width for 4 slots
        int totalContainerPadding = containerPadding * 5;
        int containerWidth = (bounds.getWidth() - totalContainerPadding) / 4;
        DBG("Container width calculated: " + juce::String(containerWidth));
        
        // Position containers based on chain configuration
        for (int slotIndex = 0; slotIndex < 4; ++slotIndex)
        {
            int effectType = chainConfig[slotIndex];
            
            // Get the container bounds for this slot
            auto slotBounds = bounds.removeFromLeft(containerWidth);
            
            // Position the appropriate container in this slot
            EffectContainer* containerToPosition = nullptr;
            
            switch (effectType)
            {
                case 1: // Bit Crusher
                    containerToPosition = bitCrusherContainer.get();
                    DBG("Positioning BitCrusher in slot " + juce::String(slotIndex));
                    break;
                case 2: // Delay
                    containerToPosition = delayContainer.get();
                    DBG("Positioning Delay in slot " + juce::String(slotIndex));
                    break;
                case 3: // EQ
                    containerToPosition = eqContainer.get();
                    DBG("Positioning EQ in slot " + juce::String(slotIndex));
                    break;
                case 4: // Reverb
                    containerToPosition = reverbContainer.get();
                    DBG("Positioning Reverb in slot " + juce::String(slotIndex));
                    break;
                default: // None (0) or invalid
                    DBG("Slot " + juce::String(slotIndex) + " is empty");
                    break;
            }
            
            // Position the container if one was selected for this slot
            if (containerToPosition != nullptr)
            {
                containerToPosition->setBounds(slotBounds);
            }
            
            // Add spacing between slots
            bounds.removeFromLeft(containerPadding);
        }
        
        DBG("=== resized() completed successfully ===");
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in resized(): " + juce::String(e.what()));
        throw;
    }
}
