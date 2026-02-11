#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Z-Filter: Combined Airwindows filter plugin
// Based on ZBandpass2, ZNotch2, ZLowpass2, ZHighpass2 by Chris Johnson (Airwindows)
// Original code: MIT License
//==============================================================================

ZFilterProcessor::ZFilterProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

ZFilterProcessor::~ZFilterProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout ZFilterProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Filter A
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("filterTypeA", 2), "Filter Type A",
        juce::StringArray{ "Lowpass", "Highpass", "Bandpass", "Notch" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("filterAEnabled", 2), "Filter A Enabled", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("polesA", 2), "Poles A",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    // Filter B
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("filterTypeB", 2), "Filter Type B",
        juce::StringArray{ "Lowpass", "Highpass", "Bandpass", "Notch" }, 2));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("filterBEnabled", 2), "Filter B Enabled", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("polesB", 2), "Poles B",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    // Routing
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("routingMode", 2), "Routing Mode", false));

    // LFO A
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lfoASpeed", 2), "LFO A Speed",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lfoADepth", 2), "LFO A Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("lfoASync", 2), "LFO A Sync", false));

    // LFO B
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lfoBSpeed", 2), "LFO B Speed",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lfoBDepth", 2), "LFO B Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("lfoBSync", 2), "LFO B Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("lfoLink", 2), "LFO Link", false));

    // Morph
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("morphEnabled", 2), "Morph Enabled", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("morph", 2), "Morph",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("morphLfoSpeed", 2), "Morph LFO Speed",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("morphLfoDepth", 2), "Morph LFO Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("morphLfoSync", 2), "Morph LFO Sync", false));

    // Master
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("frequency", 2), "Frequency",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("input", 2), "Input",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("output", 2), "Output",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 2), "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("zOutputStage", 2), "ZOutputStage", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("bypass", 2), "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("freqSmooth", 2), "Freq Smooth", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("morphSmooth2", 2), "Morph Smooth 2", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("morphSmooth3", 2), "Morph Smooth 3", false));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String ZFilterProcessor::getName() const { return JucePlugin_Name; }
bool ZFilterProcessor::acceptsMidi() const { return false; }
bool ZFilterProcessor::producesMidi() const { return false; }
bool ZFilterProcessor::isMidiEffect() const { return false; }
double ZFilterProcessor::getTailLengthSeconds() const { return 0.0; }
int ZFilterProcessor::getNumPrograms() { return 1; }
int ZFilterProcessor::getCurrentProgram() { return 0; }
void ZFilterProcessor::setCurrentProgram(int) {}
const juce::String ZFilterProcessor::getProgramName(int) { return {}; }
void ZFilterProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void ZFilterProcessor::prepareToPlay(double, int)
{
    // Reset all filter state
    std::fill(std::begin(biquadA), std::end(biquadA), 0.0);
    std::fill(std::begin(biquadB), std::end(biquadB), 0.0);
    std::fill(std::begin(biquadC), std::end(biquadC), 0.0);
    std::fill(std::begin(biquadD), std::end(biquadD), 0.0);
    std::fill(std::begin(biquadE), std::end(biquadE), 0.0);
    std::fill(std::begin(biquadA2), std::end(biquadA2), 0.0);
    std::fill(std::begin(biquadB2), std::end(biquadB2), 0.0);
    std::fill(std::begin(biquadC2), std::end(biquadC2), 0.0);
    std::fill(std::begin(biquadD2), std::end(biquadD2), 0.0);
    std::fill(std::begin(biquadE2), std::end(biquadE2), 0.0);
    std::fill(std::begin(fixA), std::end(fixA), 0.0);
    std::fill(std::begin(fixB), std::end(fixB), 0.0);

    iirSampleAL = iirSampleAR = 0.0;
    iirSampleBL = iirSampleBR = 0.0;
    morphA = morphB = 0.0;
    lfoPhaseA = 0.0;
    lfoPhaseB = 0.0;
    morphLfoPhase = 0.0;
    polesASmooth_A = polesASmooth_B = 0.0;
    polesBSmooth_A = polesBSmooth_B = 0.0;
    inTrimA = inTrimB = 1.0;
    outTrimA = outTrimB = 0.0;
    wetA = wetB = 0.0;
    mixA = mixB = 0.0;
    freqA = freqB = 0.5;

    fpdL = 1; while (fpdL < 16386) fpdL = (uint32_t)rand() * (uint32_t)UINT32_MAX;
    fpdR = 1; while (fpdR < 16386) fpdR = (uint32_t)rand() * (uint32_t)UINT32_MAX;
}

void ZFilterProcessor::releaseResources() {}

bool ZFilterProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void ZFilterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Read parameters
    const int filterTypeA = (int)*apvts.getRawParameterValue("filterTypeA");
    const int filterTypeB = (int)*apvts.getRawParameterValue("filterTypeB");
    const bool filterAOn = *apvts.getRawParameterValue("filterAEnabled") > 0.5f;
    const bool filterBOn = *apvts.getRawParameterValue("filterBEnabled") > 0.5f;
    const float polesAParam = *apvts.getRawParameterValue("polesA");
    const float polesBParam = *apvts.getRawParameterValue("polesB");
    const bool parallelRouting = *apvts.getRawParameterValue("routingMode") > 0.5f;
    const bool morphEnabled = *apvts.getRawParameterValue("morphEnabled") > 0.5f;
    const float morphParam = *apvts.getRawParameterValue("morph");
    const float morphLfoSpeedParam = *apvts.getRawParameterValue("morphLfoSpeed");
    const float morphLfoDepthParam = *apvts.getRawParameterValue("morphLfoDepth");
    const bool morphLfoSyncEnabled = *apvts.getRawParameterValue("morphLfoSync") > 0.5f;
    const bool bypassed = *apvts.getRawParameterValue("bypass") > 0.5f;
    const float mixParam = *apvts.getRawParameterValue("mix");
    const float B = *apvts.getRawParameterValue("frequency");
    const float C = *apvts.getRawParameterValue("output");
    const float inputGain = *apvts.getRawParameterValue("input");
    const bool zOutEnabled = *apvts.getRawParameterValue("zOutputStage") > 0.5f;
    const float lfoASpeedParam = *apvts.getRawParameterValue("lfoASpeed");
    const float lfoADepth = *apvts.getRawParameterValue("lfoADepth");
    const bool lfoASyncEnabled = *apvts.getRawParameterValue("lfoASync") > 0.5f;
    const float lfoBSpeedParam = *apvts.getRawParameterValue("lfoBSpeed");
    const float lfoBDepth = *apvts.getRawParameterValue("lfoBDepth");
    const bool lfoBSyncEnabled = *apvts.getRawParameterValue("lfoBSync") > 0.5f;
    const bool lfoLinked = *apvts.getRawParameterValue("lfoLink") > 0.5f;
    const bool freqSmoothEnabled = *apvts.getRawParameterValue("freqSmooth") > 0.5f;
    const bool morphSmooth2 = *apvts.getRawParameterValue("morphSmooth2") > 0.5f;
    const bool morphSmooth3 = *apvts.getRawParameterValue("morphSmooth3") > 0.5f;

    // Morph smoothing
    morphA = morphB;
    morphB = morphEnabled ? (double)morphParam : 0.0;

    // Frequency smoothing A/B rotation
    freqA = freqB;
    freqB = (double)B;

    if (bypassed)
        return;

    const int sampleFrames = buffer.getNumSamples();
    if (sampleFrames == 0) return;
    const int inFramesToProcess = sampleFrames;
    const double sr = getSampleRate();
    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= sr;

    // Helper: compute biquad freq/reso and coefficients for a given filter type
    auto computeFilterCoeffs = [&](int type, double B_val,
                                    double* bqA,
                                    double& outClipFactor, bool& outUseClip)
    {
        outClipFactor = 1.0;
        outUseClip = false;

        switch (type)
        {
            case Lowpass:
                bqA[biq_freq] = ((pow(B_val, 3) * 18930.0) / sr) + 0.00162;
                outClipFactor = 1.212 - ((1.0 - B_val) * 0.496);
                outUseClip = true;
                bqA[biq_reso] = 0.7071;
                break;
            case Highpass:
                bqA[biq_freq] = ((pow(B_val, 4) * 9500.0) / sr) + 0.00076;
                bqA[biq_reso] = 1.0;
                break;
            case Bandpass:
                bqA[biq_freq] = ((pow(B_val, 4) * 14300.0) / sr) + 0.00079;
                outClipFactor = 1.0 - ((1.0 - B_val) * 0.304);
                outUseClip = true;
                bqA[biq_reso] = 0.314;
                break;
            case Notch:
                bqA[biq_freq] = ((pow(B_val, 3) * 4700.0) / sr) + 0.0009963;
                outClipFactor = 0.91 - ((1.0 - B_val) * 0.15);
                outUseClip = true;
                bqA[biq_reso] = 0.618;
                break;
        }

        // Compute coefficients
        auto savePrev = [](double* bq) {
            bq[biq_aA0] = bq[biq_aB0]; bq[biq_aA1] = bq[biq_aB1];
            bq[biq_aA2] = bq[biq_aB2]; bq[biq_bA1] = bq[biq_bB1];
            bq[biq_bA2] = bq[biq_bB2];
        };
        savePrev(bqA);
        double Kv = tan(juce::MathConstants<double>::pi * bqA[biq_freq]);
        double normv = 1.0 / (1.0 + Kv / bqA[biq_reso] + Kv * Kv);

        switch (type) {
            case Lowpass:
                bqA[biq_aB0] = Kv * Kv * normv;
                bqA[biq_aB1] = 2.0 * bqA[biq_aB0];
                bqA[biq_aB2] = bqA[biq_aB0];
                break;
            case Highpass:
                bqA[biq_aB0] = normv;
                bqA[biq_aB1] = -2.0 * bqA[biq_aB0];
                bqA[biq_aB2] = bqA[biq_aB0];
                break;
            case Bandpass:
                bqA[biq_aB0] = Kv / bqA[biq_reso] * normv;
                bqA[biq_aB1] = 0.0;
                bqA[biq_aB2] = -bqA[biq_aB0];
                break;
            case Notch:
                bqA[biq_aB0] = (1.0 + Kv * Kv) * normv;
                bqA[biq_aB1] = 2.0 * (Kv * Kv - 1.0) * normv;
                bqA[biq_aB2] = bqA[biq_aB0];
                break;
            default: break;
        }
        bqA[biq_bB1] = 2.0 * (Kv * Kv - 1.0) * normv;
        bqA[biq_bB2] = (1.0 - Kv / bqA[biq_reso] + Kv * Kv) * normv;
    };

    // Save previous biq_freq for trim smoothing
    double prevBiqFreqA = biquadA[biq_freq];

    // Always compute coefficients for Filter A (primary arrays)
    double clipFactorA = 1.0; bool useClipA = false;
    computeFilterCoeffs(filterTypeA, (double)B,
        biquadA,
        clipFactorA, useClipA);

    // Always compute coefficients for Filter B (second arrays)
    double clipFactorB = 1.0; bool useClipB = false;
    computeFilterCoeffs(filterTypeB, (double)B,
        biquadA2,
        clipFactorB, useClipB);

    // Smoothing
    mixA = mixB;
    mixB = (double)mixParam;
    inTrimA = inTrimB;
    if (zOutEnabled) {
        double t = (double)inputGain * 10.0;
        t *= t; t *= t;
        inTrimB = t;
    } else {
        inTrimB = pow(10.0, (double)inputGain * 2.0 - 1.0);
    }
    outTrimA = outTrimB;
    if (zOutEnabled) {
        outTrimB = (double)C;
    } else {
        outTrimB = C * 10.0;
    }

    // Per-filter poles smoothing
    polesASmooth_A = polesASmooth_B;
    polesASmooth_B = pow((double)polesAParam, 2);
    polesBSmooth_A = polesBSmooth_B;
    polesBSmooth_B = pow((double)polesBParam, 2);

    double iirAmountA = 0.00069 / overallscale;
    double K, norm;
    fixA[fix_freq] = fixB[fix_freq] = 15500.0 / sr;
    fixA[fix_reso] = fixB[fix_reso] = 0.935;
    K = tan(juce::MathConstants<double>::pi * fixB[fix_freq]);
    norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
    fixA[fix_a0] = fixB[fix_a0] = K * K * norm;
    fixA[fix_a1] = fixB[fix_a1] = 2.0 * fixB[fix_a0];
    fixA[fix_a2] = fixB[fix_a2] = fixB[fix_a0];
    fixA[fix_b1] = fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
    fixA[fix_b2] = fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
    double trimOld = 0.1 + (3.712 * prevBiqFreqA);
    double trimNew = 0.1 + (3.712 * biquadA[biq_freq]);

    bool morphCoefficientBlend = morphEnabled;

    // LFO frequency computation helper
    auto computeLfoFreq = [&](float speedParam, bool syncEnabled) -> double {
        if (syncEnabled) {
            double bpm = 120.0;
            if (auto* ph = getPlayHead()) {
                if (auto pos = ph->getPosition()) {
                    if (pos->getBpm().hasValue())
                        bpm = *pos->getBpm();
                }
            }

            struct SyncDiv { const char* name; double beats; };
            static const SyncDiv divisions[] = {
                {"4/1",  16.0},    {"4/1D", 24.0},    {"4/1T", 10.667},
                {"2/1",  8.0},     {"2/1D", 12.0},    {"2/1T", 5.333},
                {"1/1",  4.0},     {"1/1D", 6.0},     {"1/1T", 2.667},
                {"1/2",  2.0},     {"1/2D", 3.0},     {"1/2T", 1.333},
                {"1/4",  1.0},     {"1/4D", 1.5},     {"1/4T", 0.667},
                {"1/8",  0.5},     {"1/8D", 0.75},    {"1/8T", 0.333},
                {"1/16", 0.25},    {"1/16D",0.375},   {"1/16T",0.167},
                {"1/32", 0.125},   {"1/32D",0.1875},  {"1/32T",0.0833},
                {"1/64", 0.0625},  {"1/64D",0.09375}, {"1/64T",0.04167},
            };
            static const int numDivisions = 27;

            int divIndex = juce::jlimit(0, numDivisions - 1,
                (int)(speedParam * (numDivisions - 1) + 0.5f));
            double beatsPerCycle = divisions[divIndex].beats;
            double secondsPerBeat = 60.0 / bpm;
            double cyclePeriod = beatsPerCycle * secondsPerBeat;
            return 1.0 / cyclePeriod;
        } else {
            return 0.01 * pow(2000.0, (double)speedParam);
        }
    };

    // LFO A frequency
    double lfoAFreqHz = computeLfoFreq(lfoASpeedParam, lfoASyncEnabled);
    double lfoAPhaseInc = lfoAFreqHz / sr;

    // LFO B frequency (uses A's settings when linked)
    float lfoBActualSpeed = lfoLinked ? lfoASpeedParam : lfoBSpeedParam;
    bool lfoBActualSync = lfoLinked ? lfoASyncEnabled : lfoBSyncEnabled;
    float lfoBActualDepth = lfoLinked ? lfoADepth : lfoBDepth;
    double lfoBFreqHz = computeLfoFreq(lfoBActualSpeed, lfoBActualSync);
    double lfoBPhaseInc = lfoBFreqHz / sr;

    // Morph LFO frequency (supports sync)
    double morphLfoFreqHz = computeLfoFreq(morphLfoSpeedParam, morphLfoSyncEnabled);
    double morphLfoPhaseInc = morphLfoFreqHz / sr;

    // Helper: compute per-sample coefficients for a given filter type at a given frequency
    auto computePerSampleCoeffs = [&](int type, double modB, double* bqA_arr) {
        double modFreq;
        switch (type) {
            case Lowpass:  modFreq = ((pow(modB, 3) * 18930.0) / sr) + 0.00162; break;
            case Highpass: modFreq = ((pow(modB, 4) * 9500.0) / sr) + 0.00076; break;
            case Bandpass: modFreq = ((pow(modB, 4) * 14300.0) / sr) + 0.00079; break;
            case Notch:    modFreq = ((pow(modB, 3) * 4700.0) / sr) + 0.0009963; break;
            default:       modFreq = bqA_arr[biq_freq]; break;
        }
        double Km = tan(juce::MathConstants<double>::pi * modFreq);
        double normm = 1.0 / (1.0 + Km / bqA_arr[biq_reso] + Km * Km);
        switch (type) {
            case Lowpass:
                bqA_arr[biq_a0] = Km * Km * normm;
                bqA_arr[biq_a1] = 2.0 * bqA_arr[biq_a0];
                bqA_arr[biq_a2] = bqA_arr[biq_a0];
                break;
            case Highpass:
                bqA_arr[biq_a0] = normm;
                bqA_arr[biq_a1] = -2.0 * bqA_arr[biq_a0];
                bqA_arr[biq_a2] = bqA_arr[biq_a0];
                break;
            case Bandpass:
                bqA_arr[biq_a0] = Km / bqA_arr[biq_reso] * normm;
                bqA_arr[biq_a1] = 0.0;
                bqA_arr[biq_a2] = -bqA_arr[biq_a0];
                break;
            case Notch:
                bqA_arr[biq_a0] = (1.0 + Km * Km) * normm;
                bqA_arr[biq_a1] = 2.0 * (Km * Km - 1.0) * normm;
                bqA_arr[biq_a2] = bqA_arr[biq_a0];
                break;
            default: break;
        }
        bqA_arr[biq_b1] = 2.0 * (Km * Km - 1.0) * normm;
        bqA_arr[biq_b2] = (1.0 - Km / bqA_arr[biq_reso] + Km * Km) * normm;
    };

    // Standard processing lambda
    auto processStandard = [&](double& smpL, double& smpR, double& dryL, double& dryR,
                               double* bqA_arr, double* bqB_arr, double* bqC_arr, double* bqD_arr,
                               double cf, bool useCF, double wetVal) {
        double bWet = 1.0, cWet = 1.0, dWet = wetVal * 4.0;
        if (dWet < 1.0)      { bWet = 0.0; cWet = 0.0; dWet = 0.0; }
        else if (dWet < 2.0) { bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0; }
        else if (dWet < 3.0) { cWet = dWet - 2.0; dWet = 0.0; }
        else                 { dWet -= 3.0; }

        double outSmp;

        // Stage A
        if (useCF) { smpL /= cf; smpR /= cf; }
        outSmp = (smpL * bqA_arr[biq_a0]) + bqA_arr[biq_sL1];
        if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
        bqA_arr[biq_sL1] = (smpL * bqA_arr[biq_a1]) - (outSmp * bqA_arr[biq_b1]) + bqA_arr[biq_sL2];
        bqA_arr[biq_sL2] = (smpL * bqA_arr[biq_a2]) - (outSmp * bqA_arr[biq_b2]);
        dryL = smpL = outSmp;

        outSmp = (smpR * bqA_arr[biq_a0]) + bqA_arr[biq_sR1];
        if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
        bqA_arr[biq_sR1] = (smpR * bqA_arr[biq_a1]) - (outSmp * bqA_arr[biq_b1]) + bqA_arr[biq_sR2];
        bqA_arr[biq_sR2] = (smpR * bqA_arr[biq_a2]) - (outSmp * bqA_arr[biq_b2]);
        dryR = smpR = outSmp;

        // Stage B
        if (bWet > 0.0) {
            if (useCF) { smpL /= cf; smpR /= cf; }
            outSmp = (smpL * bqB_arr[biq_a0]) + bqB_arr[biq_sL1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            bqB_arr[biq_sL1] = (smpL * bqB_arr[biq_a1]) - (outSmp * bqB_arr[biq_b1]) + bqB_arr[biq_sL2];
            bqB_arr[biq_sL2] = (smpL * bqB_arr[biq_a2]) - (outSmp * bqB_arr[biq_b2]);
            dryL = smpL = (outSmp * bWet) + (dryL * (1.0 - bWet));

            outSmp = (smpR * bqB_arr[biq_a0]) + bqB_arr[biq_sR1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            bqB_arr[biq_sR1] = (smpR * bqB_arr[biq_a1]) - (outSmp * bqB_arr[biq_b1]) + bqB_arr[biq_sR2];
            bqB_arr[biq_sR2] = (smpR * bqB_arr[biq_a2]) - (outSmp * bqB_arr[biq_b2]);
            dryR = smpR = (outSmp * bWet) + (dryR * (1.0 - bWet));
        }

        // Stage C
        if (cWet > 0.0) {
            if (useCF) { smpL /= cf; smpR /= cf; }
            outSmp = (smpL * bqC_arr[biq_a0]) + bqC_arr[biq_sL1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            bqC_arr[biq_sL1] = (smpL * bqC_arr[biq_a1]) - (outSmp * bqC_arr[biq_b1]) + bqC_arr[biq_sL2];
            bqC_arr[biq_sL2] = (smpL * bqC_arr[biq_a2]) - (outSmp * bqC_arr[biq_b2]);
            dryL = smpL = (outSmp * cWet) + (dryL * (1.0 - cWet));

            outSmp = (smpR * bqC_arr[biq_a0]) + bqC_arr[biq_sR1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            bqC_arr[biq_sR1] = (smpR * bqC_arr[biq_a1]) - (outSmp * bqC_arr[biq_b1]) + bqC_arr[biq_sR2];
            bqC_arr[biq_sR2] = (smpR * bqC_arr[biq_a2]) - (outSmp * bqC_arr[biq_b2]);
            dryR = smpR = (outSmp * cWet) + (dryR * (1.0 - cWet));
        }

        // Stage D
        if (dWet > 0.0) {
            if (useCF) { smpL /= cf; smpR /= cf; }
            outSmp = (smpL * bqD_arr[biq_a0]) + bqD_arr[biq_sL1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            bqD_arr[biq_sL1] = (smpL * bqD_arr[biq_a1]) - (outSmp * bqD_arr[biq_b1]) + bqD_arr[biq_sL2];
            bqD_arr[biq_sL2] = (smpL * bqD_arr[biq_a2]) - (outSmp * bqD_arr[biq_b2]);
            dryL = smpL = (outSmp * dWet) + (dryL * (1.0 - dWet));

            outSmp = (smpR * bqD_arr[biq_a0]) + bqD_arr[biq_sR1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            bqD_arr[biq_sR1] = (smpR * bqD_arr[biq_a1]) - (outSmp * bqD_arr[biq_b1]) + bqD_arr[biq_sR2];
            bqD_arr[biq_sR2] = (smpR * bqD_arr[biq_a2]) - (outSmp * bqD_arr[biq_b2]);
            dryR = smpR = (outSmp * dWet) + (dryR * (1.0 - dWet));
        }

        if (useCF) { smpL /= cf; smpR /= cf; }
    };

    // Opamp stage lambda
    auto processOpamp = [&](double& smpL, double& smpR, double outTrimVal,
                            double& iirL, double& iirR,
                            double* fxA, double* fxB) {
        double outSmp;

        if (fabs(iirL) < 1.18e-37) iirL = 0.0;
        iirL = (iirL * (1.0 - iirAmountA)) + (smpL * iirAmountA);
        smpL -= iirL;
        if (fabs(iirR) < 1.18e-37) iirR = 0.0;
        iirR = (iirR * (1.0 - iirAmountA)) + (smpR * iirAmountA);
        smpR -= iirR;

        outSmp = (smpL * fxA[fix_a0]) + fxA[fix_sL1];
        fxA[fix_sL1] = (smpL * fxA[fix_a1]) - (outSmp * fxA[fix_b1]) + fxA[fix_sL2];
        fxA[fix_sL2] = (smpL * fxA[fix_a2]) - (outSmp * fxA[fix_b2]);
        smpL = outSmp;
        outSmp = (smpR * fxA[fix_a0]) + fxA[fix_sR1];
        fxA[fix_sR1] = (smpR * fxA[fix_a1]) - (outSmp * fxA[fix_b1]) + fxA[fix_sR2];
        fxA[fix_sR2] = (smpR * fxA[fix_a2]) - (outSmp * fxA[fix_b2]);
        smpR = outSmp;

        if (smpL > 1.0) smpL = 1.0; if (smpL < -1.0) smpL = -1.0;
        smpL -= (smpL * smpL * smpL * smpL * smpL * 0.1768);
        if (smpR > 1.0) smpR = 1.0; if (smpR < -1.0) smpR = -1.0;
        smpR -= (smpR * smpR * smpR * smpR * smpR * 0.1768);

        outSmp = (smpL * fxB[fix_a0]) + fxB[fix_sL1];
        fxB[fix_sL1] = (smpL * fxB[fix_a1]) - (outSmp * fxB[fix_b1]) + fxB[fix_sL2];
        fxB[fix_sL2] = (smpL * fxB[fix_a2]) - (outSmp * fxB[fix_b2]);
        smpL = outSmp;
        outSmp = (smpR * fxB[fix_a0]) + fxB[fix_sR1];
        fxB[fix_sR1] = (smpR * fxB[fix_a1]) - (outSmp * fxB[fix_b1]) + fxB[fix_sR2];
        fxB[fix_sR2] = (smpR * fxB[fix_a2]) - (outSmp * fxB[fix_b2]);
        smpR = outSmp;

        if (outTrimVal != 1.0) {
            smpL *= outTrimVal;
            smpR *= outTrimVal;
        }
    };

    // Get channel pointers
    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);

    for (int i = 0; i < sampleFrames; ++i)
    {
        double inputSampleL = channelL[i];
        double inputSampleR = channelR[i];

        if (fabs(inputSampleL) < 1.18e-23) inputSampleL = fpdL * 1.18e-17;
        if (fabs(inputSampleR) < 1.18e-23) inputSampleR = fpdR * 1.18e-17;

        double drySampleL = inputSampleL;
        double drySampleR = inputSampleR;
        double overallDrySampleL = inputSampleL;
        double overallDrySampleR = inputSampleR;

        // Per-block coefficient interpolation factor
        double interp = (double)(sampleFrames - 1 - i) / (double)inFramesToProcess;

        double inTrim = (inTrimA * interp) + (inTrimB * (1.0 - interp));
        double outTrim = (outTrimA * interp) + (outTrimB * (1.0 - interp));
        double wetA_val = (polesASmooth_A * interp) + (polesASmooth_B * (1.0 - interp));
        double wetB_val = (polesBSmooth_A * interp) + (polesBSmooth_B * (1.0 - interp));
        double morphAmount = (morphA * interp) + (morphB * (1.0 - interp));

        double smoothedB = freqSmoothEnabled
            ? ((freqA * interp) + (freqB * (1.0 - interp)))
            : (double)B;

        // Smooth clipFactor from smoothedB
        double smoothedClipA = clipFactorA;
        double smoothedClipB = clipFactorB;
        if (freqSmoothEnabled) {
            switch (filterTypeA) {
                case Lowpass:  smoothedClipA = 1.212 - ((1.0 - smoothedB) * 0.496); break;
                case Bandpass: smoothedClipA = 1.0 - ((1.0 - smoothedB) * 0.304); break;
                case Notch:    smoothedClipA = 0.91 - ((1.0 - smoothedB) * 0.15); break;
                default: break;
            }
            switch (filterTypeB) {
                case Lowpass:  smoothedClipB = 1.212 - ((1.0 - smoothedB) * 0.496); break;
                case Bandpass: smoothedClipB = 1.0 - ((1.0 - smoothedB) * 0.304); break;
                case Notch:    smoothedClipB = 0.91 - ((1.0 - smoothedB) * 0.15); break;
                default: break;
            }
        }

        if (inTrim != 1.0) {
            inputSampleL *= inTrim;
            inputSampleR *= inTrim;
        }

        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;

        {
            double trim = freqSmoothEnabled
                ? (trimOld * interp + trimNew * (1.0 - interp))
                : trimNew;
            inputSampleL *= trim;
            inputSampleR *= trim;
        }

        // === DUAL LFO + MORPH LFO ===
        // LFO A
        double lfoAVal = sin(lfoPhaseA * 2.0 * juce::MathConstants<double>::pi);
        lfoPhaseA += lfoAPhaseInc;
        if (lfoPhaseA >= 1.0) lfoPhaseA -= 1.0;

        // LFO B (phase-lock to A when linked)
        if (lfoLinked) lfoPhaseB = lfoPhaseA;
        double lfoBVal = sin(lfoPhaseB * 2.0 * juce::MathConstants<double>::pi);
        lfoPhaseB += lfoBPhaseInc;
        if (lfoPhaseB >= 1.0) lfoPhaseB -= 1.0;

        // Morph LFO
        double morphLfoVal = sin(morphLfoPhase * 2.0 * juce::MathConstants<double>::pi);
        morphLfoPhase += morphLfoPhaseInc;
        if (morphLfoPhase >= 1.0) morphLfoPhase -= 1.0;

        // Apply Morph LFO to morph position
        double modulatedMorph = morphAmount;
        if (morphLfoDepthParam > 0.0f && morphEnabled) {
            modulatedMorph = morphAmount + morphLfoVal * (double)morphLfoDepthParam * 0.5;
            modulatedMorph = juce::jlimit(0.0, 1.0, modulatedMorph);
        }

        // Per-block coefficient interpolation helper
        auto interpBQ = [interp](double* bq) {
            bq[biq_a0] = (bq[biq_aA0] * interp) + (bq[biq_aB0] * (1.0 - interp));
            bq[biq_a1] = (bq[biq_aA1] * interp) + (bq[biq_aB1] * (1.0 - interp));
            bq[biq_a2] = (bq[biq_aA2] * interp) + (bq[biq_aB2] * (1.0 - interp));
            bq[biq_b1] = (bq[biq_bA1] * interp) + (bq[biq_bB1] * (1.0 - interp));
            bq[biq_b2] = (bq[biq_bA2] * interp) + (bq[biq_bB2] * (1.0 - interp));
        };

        // Apply LFO A to Filter A cutoff
        if (lfoADepth > 0.0f)
        {
            double modulatedBA = (double)B + lfoAVal * (double)lfoADepth * 0.5;
            modulatedBA = juce::jlimit(0.0, 1.0, modulatedBA);
            computePerSampleCoeffs(filterTypeA, modulatedBA, biquadA);
            if (!morphCoefficientBlend) {
                for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
            }
        }
        else if (freqSmoothEnabled)
        {
            // Frequency smoothing: per-sample coefficient recomputation
            computePerSampleCoeffs(filterTypeA, smoothedB, biquadA);
            if (!morphCoefficientBlend) {
                for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
            }
        }
        else
        {
            // No LFO A modulation: per-block interpolation for Filter A
            interpBQ(biquadA);
            for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
        }

        // Apply LFO B to Filter B cutoff
        if (lfoBActualDepth > 0.0f)
        {
            double modulatedBB = (double)B + lfoBVal * (double)lfoBActualDepth * 0.5;
            modulatedBB = juce::jlimit(0.0, 1.0, modulatedBB);
            computePerSampleCoeffs(filterTypeB, modulatedBB, biquadA2);
            if (!morphCoefficientBlend) {
                for (int x = 0; x < 7; x++) { biquadD2[x] = biquadC2[x] = biquadB2[x] = biquadA2[x]; }
            }
        }
        else if (freqSmoothEnabled)
        {
            // Frequency smoothing: per-sample coefficient recomputation
            computePerSampleCoeffs(filterTypeB, smoothedB, biquadA2);
            if (!morphCoefficientBlend) {
                for (int x = 0; x < 7; x++) { biquadD2[x] = biquadC2[x] = biquadB2[x] = biquadA2[x]; }
            }
        }
        else
        {
            // No LFO B modulation: per-block interpolation for Filter B
            interpBQ(biquadA2);
            for (int x = 0; x < 7; x++) { biquadD2[x] = biquadC2[x] = biquadB2[x] = biquadA2[x]; }
        }

        // Compute first-stage wet for overall dry blend (use Filter A poles)
        double aWet = 1.0;
        { double tmpWet = wetA_val * 4.0; if (tmpWet < 1.0) aWet = tmpWet; }

        // === MORPH PROCESSING ===
        if (morphCoefficientBlend && modulatedMorph > 0.0)
        {
            double m = modulatedMorph;

            if (morphSmooth2)
            {
                // === SMOOTH2: Parameter-space interpolation ===
                // Blend pre-computed frequencies and resonances from each filter's biquad arrays
                double blendFreq = biquadA[biq_freq] * (1.0 - m) + biquadA2[biq_freq] * m;
                double blendReso = biquadA[biq_reso] * (1.0 - m) + biquadA2[biq_reso] * m;

                // Compute shared denominator (always stable for positive K, reso)
                double Kb = tan(juce::MathConstants<double>::pi * blendFreq);
                double normb = 1.0 / (1.0 + Kb / blendReso + Kb * Kb);

                // Shared denominator coefficients
                double blend_b1 = 2.0 * (Kb * Kb - 1.0) * normb;
                double blend_b2 = (1.0 - Kb / blendReso + Kb * Kb) * normb;

                // Compute numerators for both types using the SAME K and norm
                double numA_a0, numA_a1, numA_a2;
                switch (filterTypeA) {
                    case Lowpass:  numA_a0 = Kb*Kb*normb; numA_a1 = 2.0*numA_a0; numA_a2 = numA_a0; break;
                    case Highpass: numA_a0 = normb; numA_a1 = -2.0*numA_a0; numA_a2 = numA_a0; break;
                    case Bandpass: numA_a0 = Kb/blendReso*normb; numA_a1 = 0.0; numA_a2 = -numA_a0; break;
                    case Notch:    numA_a0 = (1.0+Kb*Kb)*normb; numA_a1 = blend_b1; numA_a2 = numA_a0; break;
                    default:       numA_a0 = numA_a1 = numA_a2 = 0.0; break;
                }
                double numB_a0, numB_a1, numB_a2;
                switch (filterTypeB) {
                    case Lowpass:  numB_a0 = Kb*Kb*normb; numB_a1 = 2.0*numB_a0; numB_a2 = numB_a0; break;
                    case Highpass: numB_a0 = normb; numB_a1 = -2.0*numB_a0; numB_a2 = numB_a0; break;
                    case Bandpass: numB_a0 = Kb/blendReso*normb; numB_a1 = 0.0; numB_a2 = -numB_a0; break;
                    case Notch:    numB_a0 = (1.0+Kb*Kb)*normb; numB_a1 = blend_b1; numB_a2 = numB_a0; break;
                    default:       numB_a0 = numB_a1 = numB_a2 = 0.0; break;
                }

                // Blend numerators only (safe — doesn't affect stability)
                biquadA[biq_a0] = numA_a0 * (1.0 - m) + numB_a0 * m;
                biquadA[biq_a1] = numA_a1 * (1.0 - m) + numB_a1 * m;
                biquadA[biq_a2] = numA_a2 * (1.0 - m) + numB_a2 * m;
                biquadA[biq_b1] = blend_b1;
                biquadA[biq_b2] = blend_b2;
            }
            else if (morphSmooth3)
            {
                // === SMOOTH3: ARMAdillo log-domain interpolation ===
                // Transform feedback coefficients (Rossum transform)
                double b1p_A = biquadA[biq_b1] + 2.0;
                double b2p_A = 1.0 - biquadA[biq_b2];
                double b1p_B = biquadA2[biq_b1] + 2.0;
                double b2p_B = 1.0 - biquadA2[biq_b2];

                // Encode to log domain (protect against log(0))
                const double eps = 1e-20;
                double log_b1p_A = log2(fmax(b1p_A, eps));
                double log_b2p_A = log2(fmax(b2p_A, eps));
                double log_b1p_B = log2(fmax(b1p_B, eps));
                double log_b2p_B = log2(fmax(b2p_B, eps));

                // Linear interpolation in log domain
                double log_b1p = log_b1p_A * (1.0 - m) + log_b1p_B * m;
                double log_b2p = log_b2p_A * (1.0 - m) + log_b2p_B * m;

                // Decode
                double b1p = exp2(log_b1p);
                double b2p = exp2(log_b2p);

                // Reverse transform
                biquadA[biq_b1] = b1p - 2.0;
                biquadA[biq_b2] = 1.0 - b2p;

                // Numerators: linear blend (safe — doesn't affect stability)
                biquadA[biq_a0] = biquadA[biq_a0] * (1.0 - m) + biquadA2[biq_a0] * m;
                biquadA[biq_a1] = biquadA[biq_a1] * (1.0 - m) + biquadA2[biq_a1] * m;
                biquadA[biq_a2] = biquadA[biq_a2] * (1.0 - m) + biquadA2[biq_a2] * m;
            }
            else
            {
                // === Default: Raw coefficient lerp (existing behavior) ===
                double blendA0  = biquadA[biq_a0] * (1.0 - m) + biquadA2[biq_a0] * m;
                double blendA1  = biquadA[biq_a1] * (1.0 - m) + biquadA2[biq_a1] * m;
                double blendA2c = biquadA[biq_a2] * (1.0 - m) + biquadA2[biq_a2] * m;
                double blendB1  = biquadA[biq_b1] * (1.0 - m) + biquadA2[biq_b1] * m;
                double blendB2  = biquadA[biq_b2] * (1.0 - m) + biquadA2[biq_b2] * m;
                biquadA[biq_a0] = blendA0; biquadA[biq_a1] = blendA1;
                biquadA[biq_a2] = blendA2c; biquadA[biq_b1] = blendB1;
                biquadA[biq_b2] = blendB2;
            }

            // Common morph post-processing
            for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }

            double morphedPoles = wetA_val * (1.0 - m) + wetB_val * m;
            double blendClip = smoothedClipA * (1.0 - m) + smoothedClipB * m;
            bool blendUseClip = useClipA || useClipB;

            processStandard(inputSampleL, inputSampleR, drySampleL, drySampleR,
                biquadA, biquadB, biquadC, biquadD, blendClip, blendUseClip, morphedPoles);
        }
        else
        {
            // === NON-MORPH / MORPH-AT-ZERO MODE: Dual filter routing ===
            // When morph is enabled, both filters are forced active
            bool aOn = morphEnabled ? true : filterAOn;
            bool bOn = morphEnabled ? true : filterBOn;

            if (aOn && bOn)
            {
                if (parallelRouting)
                {
                    // PARALLEL: Process both chains on dry input, average outputs
                    double outAL = inputSampleL, outAR = inputSampleR;
                    double outBL = inputSampleL, outBR = inputSampleR;
                    double dryAL = outAL, dryAR = outAR;
                    double dryBL = outBL, dryBR = outBR;

                    // Process Filter A
                    processStandard(outAL, outAR, dryAL, dryAR,
                        biquadA, biquadB, biquadC, biquadD, smoothedClipA, useClipA, wetA_val);

                    // Process Filter B
                    processStandard(outBL, outBR, dryBL, dryBR,
                        biquadA2, biquadB2, biquadC2, biquadD2, smoothedClipB, useClipB, wetB_val);

                    // Average
                    inputSampleL = (outAL + outBL) * 0.5;
                    inputSampleR = (outAR + outBR) * 0.5;
                }
                else
                {
                    // SERIES: Filter A -> Filter B
                    double dryL = inputSampleL, dryR = inputSampleR;

                    // Filter A
                    processStandard(inputSampleL, inputSampleR, dryL, dryR,
                        biquadA, biquadB, biquadC, biquadD, smoothedClipA, useClipA, wetA_val);

                    // Filter B (processes output of A)
                    dryL = inputSampleL; dryR = inputSampleR;
                    processStandard(inputSampleL, inputSampleR, dryL, dryR,
                        biquadA2, biquadB2, biquadC2, biquadD2, smoothedClipB, useClipB, wetB_val);
                }
            }
            else if (aOn)
            {
                // Only Filter A
                processStandard(inputSampleL, inputSampleR, drySampleL, drySampleR,
                    biquadA, biquadB, biquadC, biquadD, smoothedClipA, useClipA, wetA_val);
            }
            else if (bOn)
            {
                // Only Filter B
                processStandard(inputSampleL, inputSampleR, drySampleL, drySampleR,
                    biquadA2, biquadB2, biquadC2, biquadD2, smoothedClipB, useClipB, wetB_val);
            }
            // else: neither filter on — signal passes through unfiltered
        }

        // Opamp stage (always uses primary opamp state)
        processOpamp(inputSampleL, inputSampleR, outTrim,
                     iirSampleAL, iirSampleAR, fixA, fixB);

        if (aWet != 1.0) {
            inputSampleL = (inputSampleL * aWet) + (overallDrySampleL * (1.0 - aWet));
            inputSampleR = (inputSampleR * aWet) + (overallDrySampleR * (1.0 - aWet));
        }

        // Dry/wet mix blend
        double mix = (mixA * interp) + (mixB * (1.0 - interp));
        if (mix < 1.0) {
            inputSampleL = (inputSampleL * mix) + (overallDrySampleL * (1.0 - mix));
            inputSampleR = (inputSampleR * mix) + (overallDrySampleR * (1.0 - mix));
        }

        // Dither
        int expon; frexpf((float)inputSampleL, &expon);
        fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
        inputSampleL += ((double(fpdL) - uint32_t(0x7fffffff)) * 5.5e-36l * pow(2, expon + 62));
        frexpf((float)inputSampleR, &expon);
        fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
        inputSampleR += ((double(fpdR) - uint32_t(0x7fffffff)) * 5.5e-36l * pow(2, expon + 62));

        channelL[i] = (float)inputSampleL;
        channelR[i] = (float)inputSampleR;
    }
}

//==============================================================================
bool ZFilterProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* ZFilterProcessor::createEditor()
{
    return new ZFilterEditor(*this);
}

//==============================================================================
void ZFilterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ZFilterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZFilterProcessor();
}
