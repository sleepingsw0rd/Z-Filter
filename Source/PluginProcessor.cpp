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
        juce::StringArray{ "Lowpass", "Highpass", "Bandpass", "Notch", "Region" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("filterAEnabled", 2), "Filter A Enabled", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("polesA", 2), "Poles A",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    // Filter B
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("filterTypeB", 2), "Filter Type B",
        juce::StringArray{ "Lowpass", "Highpass", "Bandpass", "Notch", "Region" }, 2));
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
    std::fill(std::begin(fixA2), std::end(fixA2), 0.0);
    std::fill(std::begin(fixB2), std::end(fixB2), 0.0);

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

    // Morph smoothing
    morphA = morphB;
    morphB = morphEnabled ? (double)morphParam : 0.0;

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
    auto computeFilterCoeffs = [&](int type, double B_val, double D_val,
                                    double* bqA, double* bqB, double* bqC,
                                    double* bqD, double* bqE,
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
                bqA[biq_reso] = 0.618;
                break;
            case Region:
            {
                double center = B_val;
                double spreadAmt = D_val;
                double halfSpread = spreadAmt * 0.5;
                double high = juce::jmin(center + halfSpread, 1.0);
                double low  = juce::jmax(center - halfSpread, 0.0);
                double mid  = (high + low) * 0.5;

                bqE[biq_freq] = pow(high, 3) * 20000.0 / sr;
                bqA[biq_freq] = pow((high + mid) * 0.5, 3) * 20000.0 / sr;
                bqB[biq_freq] = pow(mid, 3) * 20000.0 / sr;
                bqC[biq_freq] = pow((mid + low) * 0.5, 3) * 20000.0 / sr;
                bqD[biq_freq] = pow(low, 3) * 20000.0 / sr;

                auto clampFreq = [](double& f) { if (f < 0.00009) f = 0.00009; };
                clampFreq(bqE[biq_freq]); clampFreq(bqA[biq_freq]);
                clampFreq(bqB[biq_freq]); clampFreq(bqC[biq_freq]);
                clampFreq(bqD[biq_freq]);

                bqE[biq_reso] = bqA[biq_reso] = bqB[biq_reso] =
                    bqC[biq_reso] = bqD[biq_reso] = 0.7071;
                break;
            }
        }

        // Compute coefficients
        auto savePrev = [](double* bq) {
            bq[biq_aA0] = bq[biq_aB0]; bq[biq_aA1] = bq[biq_aB1];
            bq[biq_aA2] = bq[biq_aB2]; bq[biq_bA1] = bq[biq_bB1];
            bq[biq_bA2] = bq[biq_bB2];
        };
        auto computeBandpass = [](double* bq) {
            double Kc = tan(juce::MathConstants<double>::pi * bq[biq_freq]);
            double normC = 1.0 / (1.0 + Kc / bq[biq_reso] + Kc * Kc);
            bq[biq_aB0] = Kc / bq[biq_reso] * normC;
            bq[biq_aB1] = 0.0;
            bq[biq_aB2] = -bq[biq_aB0];
            bq[biq_bB1] = 2.0 * (Kc * Kc - 1.0) * normC;
            bq[biq_bB2] = (1.0 - Kc / bq[biq_reso] + Kc * Kc) * normC;
        };

        if (type == Region)
        {
            for (auto* bq : {bqE, bqA, bqB, bqC, bqD}) {
                savePrev(bq);
                computeBandpass(bq);
            }
        }
        else
        {
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
        }
    };

    // Always compute coefficients for Filter A (primary arrays)
    double clipFactorA = 1.0; bool useClipA = false;
    computeFilterCoeffs(filterTypeA, (double)B, (double)polesAParam,
        biquadA, biquadB, biquadC, biquadD, biquadE,
        clipFactorA, useClipA);

    // Always compute coefficients for Filter B (second arrays)
    double clipFactorB = 1.0; bool useClipB = false;
    computeFilterCoeffs(filterTypeB, (double)B, (double)polesBParam,
        biquadA2, biquadB2, biquadC2, biquadD2, biquadE2,
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
    // Copy opamp coefficients to second set for Region crossfade
    for (int x = fix_freq; x <= fix_b2; x++) {
        fixA2[x] = fixA[x];
        fixB2[x] = fixB[x];
    }

    double trim = 0.1 + (3.712 * biquadA[biq_freq]);

    // Determine if we need Region crossfade mode for morphing
    bool morphRegionCrossfade = morphEnabled &&
        (filterTypeA == Region || filterTypeB == Region);
    bool morphCoefficientBlend = morphEnabled && !morphRegionCrossfade;

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

    // Morph LFO frequency (always free-running, no sync)
    double morphLfoFreqHz = 0.01 * pow(2000.0, (double)morphLfoSpeedParam);
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

    // Helper lambdas for per-sample processing
    auto sineClip = [](double& sample, double cf) {
        sample *= cf;
        if (sample > 1.57079633) sample = 1.57079633;
        if (sample < -1.57079633) sample = -1.57079633;
        sample = sin(sample);
    };

    auto applyBiquad = [](double input, double* bq, int sIdx1, int sIdx2) -> double {
        double out = (input * bq[biq_a0]) + bq[sIdx1];
        bq[sIdx1] = (input * bq[biq_a1]) - (out * bq[biq_b1]) + bq[sIdx2];
        bq[sIdx2] = (input * bq[biq_a2]) - (out * bq[biq_b2]);
        return out;
    };

    // Region processing lambda
    auto processRegion = [&](double& smpL, double& smpR, double& dryL, double& dryR,
                             double* bqA_arr, double* bqB_arr, double* bqC_arr,
                             double* bqD_arr, double* bqE_arr,
                             double B_val, double D_val) {
        double spreadAmt = D_val;
        double halfSpread = spreadAmt * 0.5;
        double center = B_val;
        double high = juce::jmin(center + halfSpread, 1.0);
        double low  = juce::jmax(center - halfSpread, 0.0);
        double spread = 1.001 - fabs(high - low);

        double* bqs[5] = { bqE_arr, bqA_arr, bqB_arr, bqC_arr, bqD_arr };
        double cfs[5], comps[5];
        for (int s = 0; s < 5; s++) {
            cfs[s]   = 0.75 + (bqs[s][biq_freq] * spreadAmt * 37.0);
            comps[s] = sqrt(bqs[s][biq_freq]) * 6.4 * spread;
            if (comps[s] < 0.001) comps[s] = 0.001;
        }

        double raWet = 1.0, rbWet = 1.0, rcWet = 1.0, rdWet = spreadAmt * 4.0;
        if (rdWet < 1.0)      { raWet = rdWet; rbWet = 0.0; rcWet = 0.0; rdWet = 0.0; }
        else if (rdWet < 2.0) { rbWet = rdWet - 1.0; rcWet = 0.0; rdWet = 0.0; }
        else if (rdWet < 3.0) { rcWet = rdWet - 2.0; rdWet = 0.0; }
        else                  { rdWet -= 3.0; }

        // Stage E
        sineClip(smpL, cfs[0]); sineClip(smpR, cfs[0]);
        smpL = applyBiquad(smpL, bqE_arr, biq_sL1, biq_sL2) / comps[0];
        smpR = applyBiquad(smpR, bqE_arr, biq_sR1, biq_sR2) / comps[0];
        dryL = smpL; dryR = smpR;

        // Stage A
        {
            sineClip(smpL, cfs[1]); sineClip(smpR, cfs[1]);
            double outL = applyBiquad(smpL, bqA_arr, biq_sL1, biq_sL2) / comps[1];
            double outR = applyBiquad(smpR, bqA_arr, biq_sR1, biq_sR2) / comps[1];
            dryL = smpL = (outL * raWet) + (dryL * (1.0 - raWet));
            dryR = smpR = (outR * raWet) + (dryR * (1.0 - raWet));
        }
        if (rbWet > 0.0) {
            sineClip(smpL, cfs[2]); sineClip(smpR, cfs[2]);
            double outL = applyBiquad(smpL, bqB_arr, biq_sL1, biq_sL2) / comps[2];
            double outR = applyBiquad(smpR, bqB_arr, biq_sR1, biq_sR2) / comps[2];
            dryL = smpL = (outL * rbWet) + (dryL * (1.0 - rbWet));
            dryR = smpR = (outR * rbWet) + (dryR * (1.0 - rbWet));
        }
        if (rcWet > 0.0) {
            sineClip(smpL, cfs[3]); sineClip(smpR, cfs[3]);
            double outL = applyBiquad(smpL, bqC_arr, biq_sL1, biq_sL2) / comps[3];
            double outR = applyBiquad(smpR, bqC_arr, biq_sR1, biq_sR2) / comps[3];
            dryL = smpL = (outL * rcWet) + (dryL * (1.0 - rcWet));
            dryR = smpR = (outR * rcWet) + (dryR * (1.0 - rcWet));
        }
        if (rdWet > 0.0) {
            sineClip(smpL, cfs[4]); sineClip(smpR, cfs[4]);
            double outL = applyBiquad(smpL, bqD_arr, biq_sL1, biq_sL2) / comps[4];
            double outR = applyBiquad(smpR, bqD_arr, biq_sR1, biq_sR2) / comps[4];
            dryL = smpL = (outL * rdWet) + (dryL * (1.0 - rdWet));
            dryR = smpR = (outR * rdWet) + (dryR * (1.0 - rdWet));
        }
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

        if (inTrim != 1.0) {
            inputSampleL *= inTrim;
            inputSampleR *= inTrim;
        }

        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;

        inputSampleL *= trim;
        inputSampleR *= trim;

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
        if (lfoADepth > 0.0f && filterTypeA != Region)
        {
            double modulatedBA = (double)B + lfoAVal * (double)lfoADepth * 0.5;
            modulatedBA = juce::jlimit(0.0, 1.0, modulatedBA);
            computePerSampleCoeffs(filterTypeA, modulatedBA, biquadA);
            if (!morphCoefficientBlend) {
                for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
            }
        }
        else
        {
            // No LFO A modulation: per-block interpolation for Filter A
            if (filterTypeA == Region) {
                interpBQ(biquadE); interpBQ(biquadA); interpBQ(biquadB);
                interpBQ(biquadC); interpBQ(biquadD);
            } else {
                interpBQ(biquadA);
                for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
            }
        }

        // Apply LFO B to Filter B cutoff
        if (lfoBActualDepth > 0.0f && filterTypeB != Region)
        {
            double modulatedBB = (double)B + lfoBVal * (double)lfoBActualDepth * 0.5;
            modulatedBB = juce::jlimit(0.0, 1.0, modulatedBB);
            computePerSampleCoeffs(filterTypeB, modulatedBB, biquadA2);
            if (!morphCoefficientBlend) {
                for (int x = 0; x < 7; x++) { biquadD2[x] = biquadC2[x] = biquadB2[x] = biquadA2[x]; }
            }
        }
        else
        {
            // No LFO B modulation: per-block interpolation for Filter B
            if (filterTypeB == Region) {
                interpBQ(biquadE2); interpBQ(biquadA2); interpBQ(biquadB2);
                interpBQ(biquadC2); interpBQ(biquadD2);
            } else {
                interpBQ(biquadA2);
                for (int x = 0; x < 7; x++) { biquadD2[x] = biquadC2[x] = biquadB2[x] = biquadA2[x]; }
            }
        }

        // Compute first-stage wet for overall dry blend (use Filter A poles)
        double aWet = 1.0;
        { double tmpWet = wetA_val * 4.0; if (tmpWet < 1.0) aWet = tmpWet; }

        // === MORPH PROCESSING ===
        if (morphRegionCrossfade && modulatedMorph > 0.0)
        {
            // Region crossfade mode: run both chains independently, crossfade outputs
            double outAL = inputSampleL, outAR = inputSampleR;
            double outBL = inputSampleL, outBR = inputSampleR;
            double dryAL = outAL, dryAR = outAR;
            double dryBL = outBL, dryBR = outBR;

            // Process chain A
            if (filterTypeA == Region) {
                processRegion(outAL, outAR, dryAL, dryAR,
                    biquadA, biquadB, biquadC, biquadD, biquadE, (double)B, (double)polesAParam);
            } else {
                processStandard(outAL, outAR, dryAL, dryAR,
                    biquadA, biquadB, biquadC, biquadD, clipFactorA, useClipA, wetA_val);
            }

            // Process chain B
            if (filterTypeB == Region) {
                processRegion(outBL, outBR, dryBL, dryBR,
                    biquadA2, biquadB2, biquadC2, biquadD2, biquadE2, (double)B, (double)polesBParam);
            } else {
                processStandard(outBL, outBR, dryBL, dryBR,
                    biquadA2, biquadB2, biquadC2, biquadD2, clipFactorB, useClipB, wetB_val);
            }

            // Crossfade
            double m = modulatedMorph;
            inputSampleL = outAL * (1.0 - m) + outBL * m;
            inputSampleR = outAR * (1.0 - m) + outBR * m;
        }
        else if (morphCoefficientBlend && modulatedMorph > 0.0)
        {
            // Coefficient blend mode: interpolate working coefficients between A and B
            double m = modulatedMorph;

            // Blend coefficients
            double blendA0 = biquadA[biq_a0] * (1.0 - m) + biquadA2[biq_a0] * m;
            double blendA1 = biquadA[biq_a1] * (1.0 - m) + biquadA2[biq_a1] * m;
            double blendA2c = biquadA[biq_a2] * (1.0 - m) + biquadA2[biq_a2] * m;
            double blendB1 = biquadA[biq_b1] * (1.0 - m) + biquadA2[biq_b1] * m;
            double blendB2 = biquadA[biq_b2] * (1.0 - m) + biquadA2[biq_b2] * m;

            biquadA[biq_a0] = blendA0; biquadA[biq_a1] = blendA1;
            biquadA[biq_a2] = blendA2c; biquadA[biq_b1] = blendB1;
            biquadA[biq_b2] = blendB2;
            for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }

            // Interpolate poles along with morph
            double morphedPoles = wetA_val * (1.0 - m) + wetB_val * m;
            double blendClip = clipFactorA * (1.0 - m) + clipFactorB * m;
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
                    if (filterTypeA == Region) {
                        processRegion(outAL, outAR, dryAL, dryAR,
                            biquadA, biquadB, biquadC, biquadD, biquadE, (double)B, (double)polesAParam);
                    } else {
                        processStandard(outAL, outAR, dryAL, dryAR,
                            biquadA, biquadB, biquadC, biquadD, clipFactorA, useClipA, wetA_val);
                    }

                    // Process Filter B
                    if (filterTypeB == Region) {
                        processRegion(outBL, outBR, dryBL, dryBR,
                            biquadA2, biquadB2, biquadC2, biquadD2, biquadE2, (double)B, (double)polesBParam);
                    } else {
                        processStandard(outBL, outBR, dryBL, dryBR,
                            biquadA2, biquadB2, biquadC2, biquadD2, clipFactorB, useClipB, wetB_val);
                    }

                    // Average
                    inputSampleL = (outAL + outBL) * 0.5;
                    inputSampleR = (outAR + outBR) * 0.5;
                }
                else
                {
                    // SERIES: Filter A -> Filter B
                    double dryL = inputSampleL, dryR = inputSampleR;

                    // Filter A
                    if (filterTypeA == Region) {
                        processRegion(inputSampleL, inputSampleR, dryL, dryR,
                            biquadA, biquadB, biquadC, biquadD, biquadE, (double)B, (double)polesAParam);
                    } else {
                        processStandard(inputSampleL, inputSampleR, dryL, dryR,
                            biquadA, biquadB, biquadC, biquadD, clipFactorA, useClipA, wetA_val);
                    }

                    // Filter B (processes output of A)
                    dryL = inputSampleL; dryR = inputSampleR;
                    if (filterTypeB == Region) {
                        processRegion(inputSampleL, inputSampleR, dryL, dryR,
                            biquadA2, biquadB2, biquadC2, biquadD2, biquadE2, (double)B, (double)polesBParam);
                    } else {
                        processStandard(inputSampleL, inputSampleR, dryL, dryR,
                            biquadA2, biquadB2, biquadC2, biquadD2, clipFactorB, useClipB, wetB_val);
                    }
                }
            }
            else if (aOn)
            {
                // Only Filter A
                if (filterTypeA == Region) {
                    processRegion(inputSampleL, inputSampleR, drySampleL, drySampleR,
                        biquadA, biquadB, biquadC, biquadD, biquadE, (double)B, (double)polesAParam);
                } else {
                    processStandard(inputSampleL, inputSampleR, drySampleL, drySampleR,
                        biquadA, biquadB, biquadC, biquadD, clipFactorA, useClipA, wetA_val);
                }
            }
            else if (bOn)
            {
                // Only Filter B
                if (filterTypeB == Region) {
                    processRegion(inputSampleL, inputSampleR, drySampleL, drySampleR,
                        biquadA2, biquadB2, biquadC2, biquadD2, biquadE2, (double)B, (double)polesBParam);
                } else {
                    processStandard(inputSampleL, inputSampleR, drySampleL, drySampleR,
                        biquadA2, biquadB2, biquadC2, biquadD2, clipFactorB, useClipB, wetB_val);
                }
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
