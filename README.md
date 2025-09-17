# Outset-Verb
A simple reverb plugin created with JUCE to use with the Outset FM Synth.

# Version Roadmap
- 0.1 - Initial MVP of a simple reverb plugin which uses built-in [GenericAudioProcessorEditor](https://docs.juce.com/master/classGenericAudioProcessorEditor.html) and [Reverb](https://docs.juce.com/master/classReverb.html) classes
- 0.2 - Refactor Reverb to be a node on a [juce::ProcessorChain](https://docs.juce.com/master/classdsp_1_1ProcessorChain.html#details)
- 0.3 - Implement Processor Chain
- 0.3.1 - Implement Bit Chrusher effect as node on chain
- 0.4 - AudioProcessorGraph
- TBD