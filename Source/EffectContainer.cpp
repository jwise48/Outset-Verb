/*
  ==============================================================================

    EffectContainer.cpp

  ==============================================================================
*/

#include "EffectContainer.h"

//==============================================================================
EffectContainer::EffectContainer(const juce::String& title, LayoutMode mode)
    : containerTitle(title), layoutMode(mode)
{
    // Setup title label
    titleLabel.setText(containerTitle, juce::dontSendNotification);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
}

//==============================================================================
void EffectContainer::addSlider(const juce::String& parameterID, 
                                const juce::String& labelText,
                                juce::AudioProcessorValueTreeState& apvts)
{
    DBG("EffectContainer::addSlider - Parameter: " + parameterID + ", Label: " + labelText);
    
    try 
    {
        ParameterControl control;
        
        DBG("Creating slider for: " + parameterID);
        // Create slider with optimized styling for compact layout
        control.slider = std::make_unique<juce::Slider>();
        control.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        control.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 18);
        control.slider->setColour(juce::Slider::thumbColourId, juce::Colours::lightblue);
        control.slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue);
        control.slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
        DBG("Slider created successfully for: " + parameterID);
        
        DBG("Creating label for: " + parameterID);
        // Create label
        control.label = std::make_unique<juce::Label>();
        control.label->setText(labelText, juce::dontSendNotification);
        control.label->setFont(juce::Font(12.0f));
        control.label->setJustificationType(juce::Justification::centred);
        control.label->setColour(juce::Label::textColourId, juce::Colours::white);
        DBG("Label created successfully for: " + parameterID);
        
        DBG("About to create SliderAttachment for: " + parameterID);
        // Create attachment
        control.sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, parameterID, *control.slider);
        DBG("SliderAttachment created successfully for: " + parameterID);
        
        DBG("Adding components to parent for: " + parameterID);
        // Add to component and make visible
        addAndMakeVisible(*control.slider);
        addAndMakeVisible(*control.label);
        DBG("Components added to parent for: " + parameterID);
        
        DBG("Storing control for: " + parameterID);
        // Store the control
        controls.push_back(std::move(control));
        DBG("Control stored successfully for: " + parameterID);
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in EffectContainer::addSlider for " + parameterID + ": " + juce::String(e.what()));
        throw;
    }
}

void EffectContainer::addToggleButton(const juce::String& parameterID,
                                     const juce::String& labelText,
                                     juce::AudioProcessorValueTreeState& apvts)
{
    DBG("EffectContainer::addToggleButton - Parameter: " + parameterID + ", Label: " + labelText);
    
    try 
    {
        ParameterControl control;
        
        DBG("Creating toggle button for: " + parameterID);
        // Create toggle button
        control.toggleButton = std::make_unique<juce::ToggleButton>();
        control.toggleButton->setButtonText(labelText);
        control.toggleButton->setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        control.toggleButton->setColour(juce::ToggleButton::tickColourId, juce::Colours::lightblue);
        DBG("Toggle button created successfully for: " + parameterID);
        
        DBG("About to create ButtonAttachment for: " + parameterID);
        // Create attachment
        control.buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, parameterID, *control.toggleButton);
        DBG("ButtonAttachment created successfully for: " + parameterID);
        
        DBG("Adding toggle button to parent for: " + parameterID);
        // Add to component and make visible
        addAndMakeVisible(*control.toggleButton);
        DBG("Toggle button added to parent for: " + parameterID);
        
        DBG("Storing toggle control for: " + parameterID);
        // Store the control
        controls.push_back(std::move(control));
        DBG("Toggle control stored successfully for: " + parameterID);
    }
    catch (const std::exception& e)
    {
        DBG("EXCEPTION in EffectContainer::addToggleButton for " + parameterID + ": " + juce::String(e.what()));
        throw;
    }
}

//==============================================================================
void EffectContainer::paint(juce::Graphics& g)
{
    // Draw container background
    g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
    
    // Draw border
    g.setColour(juce::Colours::lightgrey.withAlpha(0.5f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 5.0f, 1.0f);
}

void EffectContainer::resized()
{
    auto bounds = getLocalBounds();

    // Position title at the top
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));

    bounds.removeFromTop(padding); // Add some spacing after title

    // Calculate layout for controls
    int numControls = static_cast<int>(controls.size());
    if (numControls == 0) return;

    // Determine if we should use 2-column layout
    bool useTwoColumns = (layoutMode == LayoutMode::TwoColumn) ||
                        (layoutMode == LayoutMode::Auto && numControls >= minControlsForTwoColumn);

    if (useTwoColumns)
    {
        // 2-column grid layout for containers with many controls (like EQ)
        int numRows = (numControls + 1) / 2;  // Ceiling division for odd numbers
        int availableHeight = bounds.getHeight() - (padding * (numRows - 1));
        int rowHeight = availableHeight / numRows;
        int columnWidth = (bounds.getWidth() - columnSpacing) / 2;

        for (int i = 0; i < numControls; ++i)
        {
            int row = i / 2;
            int col = i % 2;

            // Calculate position for this control
            int x = col * (columnWidth + columnSpacing);
            int y = row * (rowHeight + padding);
            int width = columnWidth;
            int height = rowHeight;

            juce::Rectangle<int> controlBounds(x, y, width, height);
            controlBounds = controlBounds.reduced(padding, 0);

            // Position the control
            if (controls[i].slider)
            {
                // For sliders, reserve space for label at bottom
                auto labelBounds = controlBounds.removeFromBottom(labelHeight);
                controls[i].label->setBounds(labelBounds);
                controls[i].slider->setBounds(controlBounds);
            }
            else if (controls[i].toggleButton)
            {
                // For toggle buttons, use the full space
                controls[i].toggleButton->setBounds(controlBounds);
            }
        }
    }
    else
    {
        // Single-column layout for containers with fewer controls
        int availableHeight = bounds.getHeight() - (padding * (numControls - 1));
        int controlHeightToUse = availableHeight / numControls;

        // Layout each control
        for (auto& control : controls)
        {
            auto controlBounds = bounds.removeFromTop(controlHeightToUse);
            controlBounds = controlBounds.reduced(padding, 0);

            if (control.slider)
            {
                // For sliders, reserve space for label at bottom
                auto labelBounds = controlBounds.removeFromBottom(labelHeight);
                control.label->setBounds(labelBounds);
                control.slider->setBounds(controlBounds);
            }
            else if (control.toggleButton)
            {
                // For toggle buttons, use the full space
                control.toggleButton->setBounds(controlBounds);
            }

            // Add spacing between controls
            if (&control != &controls.back())
                bounds.removeFromTop(padding);
        }
    }
}
