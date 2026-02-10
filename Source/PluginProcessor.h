#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
// Z-Filter: Combined Airwindows filter plugin
// Based on ZBandpass2, ZNotch2, ZLowpass2, ZHighpass2 by Chris Johnson (Airwindows)
// Original code: MIT License
//==============================================================================

class ZFilterProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    ZFilterProcessor();
    ~ZFilterProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

    enum FilterType { Lowpass = 0, Highpass, Bandpass, Notch };

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Biquad array indices (from Airwindows)
    enum {
        biq_freq, biq_reso, biq_a0, biq_a1, biq_a2, biq_b1, biq_b2,
        biq_aA0, biq_aA1, biq_aA2, biq_bA1, biq_bA2,
        biq_aB0, biq_aB1, biq_aB2, biq_bB1, biq_bB2,
        biq_sL1, biq_sL2, biq_sR1, biq_sR2,
        biq_total
    };

    // Fix (opamp) array indices
    enum {
        fix_freq, fix_reso, fix_a0, fix_a1, fix_a2, fix_b1, fix_b2,
        fix_sL1, fix_sL2, fix_sR1, fix_sR2,
        fix_total
    };

    // Filter state arrays (Filter A / primary)
    double biquadA[biq_total] = {};
    double biquadB[biq_total] = {};
    double biquadC[biq_total] = {};
    double biquadD[biq_total] = {};
    double biquadE[biq_total] = {};

    // Second filter (B) biquad arrays for morphing
    double biquadA2[biq_total] = {};
    double biquadB2[biq_total] = {};
    double biquadC2[biq_total] = {};
    double biquadD2[biq_total] = {};
    double biquadE2[biq_total] = {};

    // Opamp stage state
    double fixA[fix_total] = {};
    double fixB[fix_total] = {};

    // IIR state
    double iirSampleAL = 0.0;
    double iirSampleAR = 0.0;
    double iirSampleBL = 0.0;
    double iirSampleBR = 0.0;

    // Smoothing state
    double inTrimA = 0.0, inTrimB = 0.0;
    double outTrimA = 0.0, outTrimB = 0.0;
    double wetA = 0.0, wetB = 0.0;
    double mixA = 0.0, mixB = 0.0;

    // Morph smoothing state
    double morphA = 0.0, morphB = 0.0;

    // LFO state
    double lfoPhaseA = 0.0;
    double lfoPhaseB = 0.0;
    double morphLfoPhase = 0.0;

    // Per-filter poles smoothing
    double polesASmooth_A = 0.0, polesASmooth_B = 0.0;
    double polesBSmooth_A = 0.0, polesBSmooth_B = 0.0;

    // Frequency smoothing state
    double freqA = 0.5, freqB = 0.5;

    // Dither state
    uint32_t fpdL = 1, fpdR = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZFilterProcessor)
};
