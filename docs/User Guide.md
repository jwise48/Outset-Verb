# Outset-Verb Audio Plugin Development Guide

## Table of Contents

- [Introduction](#introduction)
  - [What is Outset-Verb?](#what-is-outset-verb)
  - [Key Features](#key-features)
  - [Target Audience](#target-audience)
  - [Prerequisites](#prerequisites)

- [Core JUCE Concepts](#core-juce-concepts)
  - [juce::AudioProcessor](#juceaudioprocessor)
  - [juce::AudioProcessorValueTreeState](#juceaudioprocessortreestate)
  - [juce::AudioProcessorEditor](#juceaudioprocessoreditor)
  - [juce::AudioProcessorValueTreeState::ParameterLayout](#juceaudioprocessortreestateparameterlayout)
  - [JUCE DSP Framework](#juce-dsp-framework)

- [Outset-Verb Architecture](#outset-verb-architecture)
  - [Dynamic Processor Chain](#dynamic-processor-chain)
  - [EffectContainer System](#effectcontainer-system)
  - [Individual Effect Nodes](#individual-effect-nodes)
  - [Parameter Management](#parameter-management)

- [Effect Algorithms](#effect-algorithms)
  - [Bit Crusher](#bit-crusher)
  - [Delay](#delay)
  - [Three-Band EQ](#three-band-eq)
  - [Reverb](#reverb)

- [Custom Classes](#custom-classes)
  - [EffectContainer](#effectcontainer)
  - [BitCrusherNode](#bitcrushernode)
  - [DelayNode](#delaynode)
  - [ThreeBandEQNode](#threebandeqnode)
  - [ReverbNode](#reverbnode)

- [Implementation Guide](#implementation-guide)
  - [Step 1: Project Setup](#step-1-project-setup)
  - [Step 2: Basic Plugin Structure](#step-2-basic-plugin-structure)
  - [Step 3: Effect Node Implementation](#step-3-effect-node-implementation)
  - [Step 4: UI Development](#step-4-ui-development)
  - [Step 5: Dynamic Chain System](#step-5-dynamic-chain-system)
  - [Step 6: Advanced Features](#step-6-advanced-features)

- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)
- [Further Reading](#further-reading)

---

## Introduction

### What is Outset-Verb?

Outset-Verb is a multi-effect audio plugin built using JUCE (Jules' Utility Class Extensions), a cross-platform C++ framework for developing audio applications. The plugin implements four distinct audio effects - bit crushing, delay, equalization, and reverb - that can be arranged in any order through a dynamic processor chain system.

The plugin serves as an excellent example of modern JUCE plugin development, demonstrating best practices for:
- Parameter management with AudioProcessorValueTreeState
- Custom DSP processor implementation
- Dynamic UI component management
- Real-time audio processing
- State persistence and recall

### Key Features

- **Four Audio Effects**: Bit Crusher, Delay, Three-Band EQ, and Reverb
- **Dynamic Chain Ordering**: Users can arrange effects in any sequence
- **Real-time Parameter Control**: All parameters are automatable and respond in real-time
- **Visual Feedback**: Effect containers grey out when not active in the chain
- **State Persistence**: Plugin settings are saved and recalled automatically
- **Cross-platform Compatibility**: Works on Windows, macOS, and Linux

### Target Audience

This guide is written for developers who are:
- New to JUCE plugin development
- Familiar with C++ programming
- Interested in audio DSP concepts
- Looking to understand modern plugin architecture patterns

### Prerequisites

Before following this guide, you should have:
- Basic C++ knowledge (classes, inheritance, templates)
- Understanding of digital audio concepts (sample rate, bit depth, stereo)
- Familiarity with digital signal processing fundamentals
- JUCE framework installed and configured
- A C++ development environment (Visual Studio, Xcode, etc.)

---

## Core JUCE Concepts

### juce::AudioProcessor

The `juce::AudioProcessor` class is the foundation of all JUCE audio plugins. It defines the interface that audio hosts use to communicate with your plugin.

**Key Responsibilities:**
- Audio processing (`processBlock()`)
- Parameter management
- Plugin metadata (name, I/O configuration)
- State persistence
- Editor creation

**Documentation Link:** [JUCE AudioProcessor Documentation](https://docs.juce.com/master/classAudioProcessor.html)

**Basic Implementation:**
```cpp
class MyAudioProcessor : public juce::AudioProcessor
{
public:
    void processBlock(juce::AudioBuffer<float>& buffer,
                     juce::MidiBuffer& midiMessages) override
    {
        // Your audio processing code here
    }

    juce::AudioProcessorEditor* createEditor() override
    {
        return new MyAudioProcessorEditor(*this);
    }

    // ... other required methods
};
```

### juce::AudioProcessorValueTreeState

`juce::AudioProcessorValueTreeState` (APVTS) is JUCE's modern parameter management system. It provides:
- Type-safe parameter definitions
- Automatic state serialization
- Parameter-to-UI binding
- Undo/redo support
- Host automation integration

**Key Features:**
- Thread-safe parameter access
- Automatic parameter smoothing
- Listener pattern for parameter changes
- Integration with host automation systems

**Documentation Link:** [JUCE AudioProcessorValueTreeState Documentation](https://docs.juce.com/master/classAudioProcessorValueTreeState.html)

**Parameter Definition Example:**
```cpp
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("gain", 1),
        "Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    return layout;
}
```

### juce::AudioProcessorEditor

The `juce::AudioProcessorEditor` class handles the graphical user interface of your plugin. It provides:
- Component-based UI construction
- Event handling (mouse, keyboard)
- Painting and graphics
- Layout management
- Integration with the host window system

**Documentation Link:** [JUCE AudioProcessorEditor Documentation](https://docs.juce.com/master/classAudioProcessorEditor.html)

**Basic Editor Structure:**
```cpp
class MyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MyAudioProcessorEditor(MyAudioProcessor& p)
        : AudioProcessorEditor(&p), processor(p)
    {
        setSize(400, 300);
        // Add UI components here
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
    }

    void resized() override
    {
        // Layout components here
    }

private:
    MyAudioProcessor& processor;
};
```

### juce::AudioProcessorValueTreeState::ParameterLayout

The ParameterLayout class defines the structure and properties of your plugin's parameters. Each parameter has:
- Unique identifier
- Display name
- Value range and default
- Optional string conversion functions

**Parameter Types:**
- `AudioParameterFloat` - Continuous floating-point values
- `AudioParameterInt` - Integer values
- `AudioParameterBool` - Boolean on/off parameters
- `AudioParameterChoice` - Selection from predefined options

**Documentation Link:** [JUCE ParameterLayout Documentation](https://docs.juce.com/master/classAudioProcessorValueTreeState_1_1ParameterLayout.html)

### JUCE DSP Framework

JUCE provides a comprehensive DSP framework for audio processing:

**Key Classes:**
- `juce::dsp::ProcessorChain` - Sequential processing of multiple effects
- `juce::dsp::ProcessSpec` - Audio processing specifications
- `juce::dsp::ProcessContext` - Processing context with input/output blocks
- Individual processors (IIR filters, delays, etc.)

**Documentation Link:** [JUCE DSP Module Documentation](https://docs.juce.com/master/group__juce__dsp.html)

**Processing Context Usage:**
```cpp
void process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& block = context.getOutputBlock();
    // Process audio block
}
```

---

## Outset-Verb Architecture

### Dynamic Processor Chain

Outset-Verb implements a dynamic processor chain that allows users to arrange effects in any order. Unlike JUCE's `ProcessorChain` class which has a fixed order, this system uses individual effect processors that can be routed dynamically.

**Key Components:**
- Individual effect processors (BitCrusherNode, DelayNode, etc.)
- Chain configuration array storing effect order
- Dynamic routing in `processBlock()`
- UI controls for chain ordering

**Benefits:**
- Flexible effect ordering
- Individual effect bypass
- Visual feedback for active effects
- Extensible design for adding new effects

### EffectContainer System

The EffectContainer class provides a modular UI component for grouping effect parameters. It features:
- Automatic layout management
- Parameter-to-control binding
- Visual enabled/disabled states
- Consistent styling across effects

**Features:**
- Single-column or two-column layouts
- Slider and toggle button support
- APVTS integration
- Responsive visual feedback

### Individual Effect Nodes

Each effect is implemented as a separate DSP processor class:
- `BitCrusherNode` - Bit depth reduction and sample rate decimation
- `DelayNode` - Digital delay with feedback and filtering
- `ThreeBandEQNode` - Three-band parametric equalizer
- `ReverbNode` - Algorithmic reverb wrapper

**Common Interface:**
- `prepare()` - Initialize with sample rate and buffer size
- `reset()` - Clear internal state
- `process()` - Template-based audio processing
- Parameter setter methods

### Parameter Management

Outset-Verb uses a comprehensive parameter system with:
- 4 chain ordering parameters (chainSlot1-4)
- Effect-specific parameters for each processor
- Real-time parameter updates
- State persistence

**Parameter Categories:**
- **Chain Configuration:** Effect ordering and selection
- **Bit Crusher:** Bit depth, sample rate reduction, mix
- **Delay:** Time, feedback, mix, low-pass cutoff
- **EQ:** Low/mid/high gain, frequency, Q factor
- **Reverb:** Room size, damping, mix, width, freeze mode

---

## Effect Algorithms

### Bit Crusher

The bit crusher effect simulates vintage digital audio equipment by reducing bit depth and sample rate.

**Algorithm Overview:**
1. **Bit Depth Reduction:** Quantizes the audio signal to fewer bits
2. **Sample Rate Decimation:** Holds samples for multiple input samples
3. **Wet/Dry Mixing:** Blends processed and original signals

**Mathematical Equations:**

Bit depth quantization:
```
output = round(input * (2^(bitDepth-1))) / (2^(bitDepth-1))
```

Sample rate reduction (sample and hold):
```
if (sampleCounter >= reductionFactor) {
    holdValue = input;
    sampleCounter = 0;
}
output = holdValue;
sampleCounter++;
```

**Implementation Details:**
- Supports 1-16 bit depth reduction
- Sample rate reduction factor of 1-50
- Per-channel sample and hold state
- Wet/dry mix control

**Audio Flow Diagram:**
```
Input → Bit Depth Quantization → Sample Rate Decimation → Mix → Output
```

### Delay

The delay effect implements a digital delay line with feedback and low-pass filtering.

**Algorithm Overview:**
1. **Delay Line:** Circular buffer storing delayed samples
2. **Feedback Loop:** Routes output back to input
3. **Low-Pass Filtering:** Softens feedback tone
4. **Wet/Dry Mixing:** Blends delayed and original signals

**Mathematical Equations:**

Delay time calculation:
```
delayInSamples = (delayTimeMs / 1000) * sampleRate
```

Feedback processing:
```
delayedSample = delayLine.read(delayInSamples)
filteredSample = lowPassFilter.processSample(delayedSample)
output = input + (filteredSample * feedback)
delayLine.write(output)
```

**Implementation Details:**
- Maximum delay time: 2000ms (96000 samples at 48kHz)
- Feedback range: 0.0 - 0.95 (to prevent runaway)
- Low-pass cutoff: 200Hz - 20kHz
- Per-channel delay lines and filters

**Audio Flow Diagram:**
```
Input → Delay Line → Low-Pass Filter → Feedback Gain → Mix → Output
    ↑                                                            │
    └──────────────────── Feedback Loop ─────────────────────────┘
```

### Three-Band EQ

The three-band EQ implements low shelf, parametric mid, and high shelf filters.

**Algorithm Overview:**
1. **Low Shelf Filter:** Boosts or cuts frequencies below the cutoff
2. **Parametric Mid Filter:** Boosts or cuts a specific frequency range
3. **High Shelf Filter:** Boosts or cuts frequencies above the cutoff
4. **Serial Processing:** Filters applied in sequence

**Mathematical Equations:**

Low shelf filter transfer function:
```
H(s) = A * (s^2 + (√A/√2) * s + A) / (A * s^2 + (√A/√2) * s + 1)
where A = 10^(gain/40)
```

Parametric mid filter transfer function:
```
H(s) = (s^2 + (gain * s/Q) + 1) / (s^2 + (s/Q) + 1)
```

High shelf filter transfer function:
```
H(s) = A * (A * s^2 + (√A/√2) * s + 1) / (s^2 + (√A/√2) * s + A)
```

**Implementation Details:**
- Low band: 20-500 Hz, ±12 dB gain
- Mid band: 200-5000 Hz, ±12 dB gain, Q: 0.1-10
- High band: 2000-20000 Hz, ±12 dB gain
- IIR filter implementation per channel

**Audio Flow Diagram:**
```
Input → Low Shelf → Parametric Mid → High Shelf → Output
```

### Reverb

The reverb effect uses JUCE's built-in algorithmic reverb processor.

**Algorithm Overview:**
JUCE's reverb implements a feedback delay network (FDN) with:
- Multiple delay lines of different lengths
- Feedback matrix for diffusion
- Low-pass filtering for high-frequency damping
- Early reflection simulation

**Key Parameters:**
- **Room Size:** Controls delay line lengths (0.0-1.0)
- **Damping:** High-frequency absorption (0.0-1.0)
- **Width:** Stereo spread (0.0-1.0)
- **Freeze Mode:** Infinite sustain effect

**Implementation Details:**
- Wraps `juce::Reverb` class
- Stereo processing with mono fallback
- Wet/dry mix control
- Parameter range conversion for intuitive control

**Audio Flow Diagram:**
```
Input → Reverb Processor → Mix → Output
```

---

## Custom Classes

### EffectContainer

A reusable UI component for organizing effect parameters.

**Key Features:**
- Automatic parameter binding
- Responsive layout (single/two-column)
- Visual enabled/disabled states
- Consistent styling

**Usage:**
```cpp
EffectContainer bitCrusherContainer("Bit Crusher", EffectContainer::LayoutMode::Auto);
bitCrusherContainer.addSlider("bitDepth", "Bit Depth", apvts);
bitCrusherContainer.addSlider("sampleRateReduction", "Sample Rate Reduction", apvts);
bitCrusherContainer.addSlider("bitCrusherMix", "Mix", apvts);
```

### BitCrusherNode

Implements bit crushing and sample rate reduction effects.

**Parameters:**
- Bit depth (1-16 bits)
- Sample rate reduction (1-50x)
- Mix (0.0-1.0)

**Processing Template:**
```cpp
template<typename ProcessContext>
void process(const ProcessContext& context) noexcept
{
    // Bit depth and sample rate processing
}
```

### DelayNode

Implements digital delay with feedback and filtering.

**Parameters:**
- Delay time (0-2000ms)
- Feedback (0.0-0.95)
- Mix (0.0-1.0)
- Low-pass cutoff (200-20000Hz)

**Internal Components:**
- DelayLine for each channel
- IIR low-pass filter for feedback

### ThreeBandEQNode

Implements three-band parametric equalization.

**Parameters:**
- Low gain/frequency
- Mid gain/frequency/Q
- High gain/frequency

**Internal Components:**
- IIR low shelf filter
- IIR parametric filter
- IIR high shelf filter

### ReverbNode

Wrapper for JUCE's reverb processor.

**Parameters:**
- Room size (0.0-1.0)
- Damping (0.0-1.0)
- Mix (0.0-1.0)
- Width (0.0-1.0)
- Freeze mode (bool)

---

## Implementation Guide

### Step 1: Project Setup

1. **Create New JUCE Project:**
   - Use Projucer to create a new Audio Plugin project
   - Configure plugin format (VST3, AU, etc.)
   - Set plugin name and manufacturer details

2. **Project Structure:**
   ```
   Source/
   ├── PluginProcessor.h/cpp
   ├── PluginEditor.h/cpp
   ├── Effects/
   │   ├── BitCrusherNode.h/cpp
   │   ├── DelayNode.h/cpp
   │   ├── ThreeBandEQNode.h/cpp
   │   └── ReverbNode.h/cpp
   └── EffectContainer.h/cpp
   ```

3. **Include Paths:**
   - Add JUCE modules: audio_basics, audio_processors, audio_utils, core, data_structures, dsp, events, graphics, gui_basics, gui_extra

### Step 2: Basic Plugin Structure

1. **Implement AudioProcessor:**
   ```cpp
   class OutsetVerbAudioProcessor : public juce::AudioProcessor
   {
   public:
       OutsetVerbAudioProcessor();
       ~OutsetVerbAudioProcessor() override;

       void prepareToPlay(double, int) override;
       void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
       juce::AudioProcessorEditor* createEditor() override;

       static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
       std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;

   private:
       JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutsetVerbAudioProcessor)
   };
   ```

2. **Create Parameter Layout:**
   - Define all parameters in `createParameterLayout()`
   - Use appropriate parameter types (Float, Bool, Choice)
   - Set meaningful ranges and defaults

3. **Initialize APVTS:**
   ```cpp
   apvts = std::make_unique<juce::AudioProcessorValueTreeState>(
       *this, nullptr, "Parameters", createParameterLayout());
   ```

### Step 3: Effect Node Implementation

1. **Create Base Effect Interface:**
   ```cpp
   class EffectNode
   {
   public:
       virtual void prepare(const juce::dsp::ProcessSpec&) = 0;
       virtual void reset() = 0;
       template<typename ProcessContext>
       void process(const ProcessContext&) = 0;
       virtual ~EffectNode() = default;
   };
   ```

2. **Implement Each Effect:**
   - Follow the template pattern for `process()` method
   - Use JUCE DSP classes where appropriate
   - Implement parameter setters
   - Handle multi-channel processing

3. **Bit Crusher Example:**
   ```cpp
   void BitCrusherNode::setBitDepth(float depth)
   {
       bitDepth = depth;
   }

   template<typename ProcessContext>
   void BitCrusherNode::process(const ProcessContext& context) noexcept
   {
       // Implementation here
   }
   ```

### Step 4: UI Development

1. **Create PluginEditor:**
   ```cpp
   class OutsetVerbAudioProcessorEditor : public juce::AudioProcessorEditor
   {
   public:
       OutsetVerbAudioProcessorEditor(OutsetVerbAudioProcessor&);
       void paint(juce::Graphics&) override;
       void resized() override;

   private:
       OutsetVerbAudioProcessor& processor;
       // UI components here
   };
   ```

2. **Implement EffectContainer:**
   - Add parameter binding methods
   - Implement automatic layout
   - Add visual state management

3. **Layout Components:**
   - Chain ordering UI at top
   - Effect containers below
   - Proper spacing and alignment

### Step 5: Dynamic Chain System

1. **Add Chain Configuration:**
   ```cpp
   std::array<int, 4> chainConfiguration = {0, 0, 0, 0};
   enum EffectType { none = 0, bitCrusher = 1, delay = 2, eq = 3, reverb = 4 };
   ```

2. **Implement Dynamic Processing:**
   ```cpp
   void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
   {
       updateChainParameters();

       juce::dsp::AudioBlock<float> audioBlock(buffer);

       for (int slot = 0; slot < 4; ++slot)
       {
           int effectType = chainConfiguration[slot];
           if (effectType == EffectType::none) continue;

           juce::dsp::ProcessContextReplacing<float> context(audioBlock);

           switch (effectType)
           {
               case EffectType::bitCrusher:
                   bitCrusherProcessor.process(context);
                   break;
               // ... other effects
           }
       }
   }
   ```

3. **Add Chain UI:**
   - ComboBox components for each slot
   - Parameter attachments
   - Visual flow indicators

### Step 6: Advanced Features

1. **State Persistence:**
   - Implement `getStateInformation()` and `setStateInformation()`
   - Use APVTS state serialization

2. **Parameter Validation:**
   - Prevent duplicate effects in chain
   - Validate parameter ranges

3. **Performance Optimization:**
   - Use `ScopedNoDenormals` in `processBlock()`
   - Minimize allocations in audio thread
   - Use efficient algorithms

---

## Best Practices

### Code Organization
- Separate DSP logic from UI code
- Use consistent naming conventions
- Document complex algorithms
- Keep methods focused and single-purpose

### Performance
- Avoid allocations in `processBlock()`
- Use template-based processing where possible
- Cache expensive calculations
- Profile and optimize critical paths

### User Experience
- Provide sensible parameter defaults
- Use appropriate parameter ranges
- Implement visual feedback
- Test with various hosts and configurations

### Maintenance
- Keep JUCE version compatibility in mind
- Document API changes
- Use version control effectively
- Write unit tests for critical components

---

## Troubleshooting

### Common Issues

**Plugin doesn't load:**
- Check JUCE module includes
- Verify plugin format configuration
- Ensure correct inheritance from AudioProcessor

**Audio artifacts:**
- Check for denormals in processing
- Verify buffer sizes and channel counts
- Test parameter smoothing

**UI doesn't update:**
- Verify APVTS parameter IDs match
- Check attachment initialization
- Test parameter change listeners

**Memory leaks:**
- Use RAII principles
- Check for circular references
- Use JUCE leak detection in debug builds

### Debugging Tips
- Use `DBG()` macro for logging
- Implement parameter monitoring
- Test with different sample rates
- Use audio analysis tools

---

## Further Reading

### JUCE Documentation
- [JUCE Tutorials](https://docs.juce.com/master/tutorial/index.html)
- [JUCE API Reference](https://docs.juce.com/master/index.html)
- [JUCE DSP Examples](https://docs.juce.com/master/group__juce__dsp.html)

### Audio DSP Resources
- "The Computer Music Tutorial" by Curtis Roads
- "Digital Audio Effects" by Udo Zölzer
- JUCE forum discussions

### Development Tools
- [Projucer](https://docs.juce.com/master/tutorial_new_projucer_project.html)
- [JUCE CMake Integration](https://docs.juce.com/master/tutorial_cmake.html)
- Audio plugin hosts (Reaper, Ableton Live, etc.)

---

*This guide was created based on the Outset-Verb plugin implementation. For the latest code examples and updates, refer to the project repository.*
