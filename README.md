# Outset-Verb

A multi-effect audio plugin built with JUCE, featuring dynamic effect chaining and four professional-grade audio effects.

## Features

- **Four Audio Effects**: Bit Crusher, Delay, Three-Band EQ, and Reverb
- **Dynamic Chain Ordering**: Arrange effects in any sequence with real-time reordering
- **Professional UI**: Clean interface with visual feedback for active effects
- **Cross-platform**: Compatible with Windows, macOS, and Linux
- **State Persistence**: Automatic saving and loading of plugin settings
- **Real-time Processing**: All parameters are automatable and respond instantly

## Building

### Prerequisites

- JUCE framework (version 7.0 or later)
- C++17 compatible compiler
- CMake or Projucer for project configuration

### Build Steps

1. Clone the repository
2. Open `Outset-Verb.jucer` in Projucer
3. Configure your build settings and export
4. Build the generated project files

## Usage

Load Outset-Verb in your favorite DAW as a VST3, AU, or AAX plugin. Use the chain ordering dropdowns at the top to arrange effects, then adjust parameters in each effect container.

## Documentation

For detailed information about the implementation, architecture, and step-by-step development guide, see:

**[📖 User Guide](docs/User%20Guide.md)** - Comprehensive development guide covering JUCE concepts, DSP algorithms, and implementation details

**[🔧 Dynamic Processor Chain Design Doc](docs/Dynamic%20Processor%20Chain%20Design%20Doc.md)** - Technical design document for the dynamic effect chaining system

## Project Structure

```
Outset-Verb/
├── Source/
│   ├── PluginProcessor.h/cpp      # Main audio processor
│   ├── PluginEditor.h/cpp         # UI implementation
│   ├── EffectContainer.h/cpp      # Reusable UI component
│   └── Effects/                   # Individual effect processors
│       ├── BitCrusherNode.h/cpp
│       ├── DelayNode.h/cpp
│       ├── ThreeBandEQNode.h/cpp
│       └── ReverbNode.h/cpp
├── docs/                          # Documentation
│   ├── User Guide.md
│   └── Dynamic Processor Chain Design Doc.md
├── Outset-Verb.jucer             # Projucer project file
└── README.md
```

## License

This project is open source. See individual files for license information.

## Contributing

Contributions are welcome! Please see the User Guide for development guidelines and best practices.
