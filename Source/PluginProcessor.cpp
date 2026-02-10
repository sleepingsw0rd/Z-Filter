#include "PluginProcessor.h"
#include "PluginEditor.h"

ZFilterMiniProcessor::ZFilterMiniProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

ZFilterMiniProcessor::~ZFilterMiniProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout ZFilterMiniProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("filterType", 1), "Filter Type",
        juce::StringArray{ "Lowpass", "Highpass", "Bandpass", "Notch" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("frequency", 1), "Frequency",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("poles", 1), "Poles",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("input", 1), "Input",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("output", 1), "Output",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 1), "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("bypass", 1), "Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("freqSmooth", 1), "Freq Smooth", false));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String ZFilterMiniProcessor::getName() const { return JucePlugin_Name; }
bool ZFilterMiniProcessor::acceptsMidi() const { return false; }
bool ZFilterMiniProcessor::producesMidi() const { return false; }
bool ZFilterMiniProcessor::isMidiEffect() const { return false; }
double ZFilterMiniProcessor::getTailLengthSeconds() const { return 0.0; }
int ZFilterMiniProcessor::getNumPrograms() { return 1; }
int ZFilterMiniProcessor::getCurrentProgram() { return 0; }
void ZFilterMiniProcessor::setCurrentProgram(int) {}
const juce::String ZFilterMiniProcessor::getProgramName(int) { return {}; }
void ZFilterMiniProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void ZFilterMiniProcessor::prepareToPlay(double, int)
{
    std::fill(std::begin(biquadA), std::end(biquadA), 0.0);
    std::fill(std::begin(biquadB), std::end(biquadB), 0.0);
    std::fill(std::begin(biquadC), std::end(biquadC), 0.0);
    std::fill(std::begin(biquadD), std::end(biquadD), 0.0);
    std::fill(std::begin(fixA), std::end(fixA), 0.0);
    std::fill(std::begin(fixB), std::end(fixB), 0.0);

    iirSampleAL = iirSampleAR = 0.0;
    inTrimA = inTrimB = 0.0;
    outTrimA = outTrimB = 0.0;
    wetA = wetB = 0.0;
    mixA = mixB = 0.0;
    freqA = freqB = 0.5;

    fpdL = 1; while (fpdL < 16386) fpdL = (uint32_t)rand() * (uint32_t)UINT32_MAX;
    fpdR = 1; while (fpdR < 16386) fpdR = (uint32_t)rand() * (uint32_t)UINT32_MAX;
}

void ZFilterMiniProcessor::releaseResources() {}

bool ZFilterMiniProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void ZFilterMiniProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Read parameters
    const int filterType = (int)*apvts.getRawParameterValue("filterType");
    const bool bypassed = *apvts.getRawParameterValue("bypass") > 0.5f;
    const bool freqSmoothEnabled = *apvts.getRawParameterValue("freqSmooth") > 0.5f;
    const float mixParam = *apvts.getRawParameterValue("mix");
    const float A = *apvts.getRawParameterValue("input");
    const float B = *apvts.getRawParameterValue("frequency");
    const float C = *apvts.getRawParameterValue("output");
    const float D = *apvts.getRawParameterValue("poles");

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

    // Save previous biq_freq for trim smoothing
    double prevBiqFreqA = biquadA[biq_freq];

    // Compute biquad coefficients
    double clipFactor = 1.0;
    bool useClip = false;

    switch (filterType)
    {
        case Lowpass:
            biquadA[biq_freq] = ((pow((double)B, 3) * 18930.0) / sr) + 0.00162;
            clipFactor = 1.212 - ((1.0 - (double)B) * 0.496);
            useClip = true;
            biquadA[biq_reso] = 0.7071;
            break;
        case Highpass:
            biquadA[biq_freq] = ((pow((double)B, 4) * 9500.0) / sr) + 0.00076;
            biquadA[biq_reso] = 1.0;
            break;
        case Bandpass:
            biquadA[biq_freq] = ((pow((double)B, 4) * 14300.0) / sr) + 0.00079;
            clipFactor = 1.0 - ((1.0 - (double)B) * 0.304);
            useClip = true;
            biquadA[biq_reso] = 0.314;
            break;
        case Notch:
            biquadA[biq_freq] = ((pow((double)B, 3) * 4700.0) / sr) + 0.0009963;
            clipFactor = 0.91 - ((1.0 - (double)B) * 0.15);
            useClip = true;
            biquadA[biq_reso] = 0.618;
            break;
    }

    // Save previous coefficients for interpolation
    biquadA[biq_aA0] = biquadA[biq_aB0]; biquadA[biq_aA1] = biquadA[biq_aB1];
    biquadA[biq_aA2] = biquadA[biq_aB2]; biquadA[biq_bA1] = biquadA[biq_bB1];
    biquadA[biq_bA2] = biquadA[biq_bB2];

    // Compute new coefficients
    double Kv = tan(juce::MathConstants<double>::pi * biquadA[biq_freq]);
    double normv = 1.0 / (1.0 + Kv / biquadA[biq_reso] + Kv * Kv);

    switch (filterType) {
        case Lowpass:
            biquadA[biq_aB0] = Kv * Kv * normv;
            biquadA[biq_aB1] = 2.0 * biquadA[biq_aB0];
            biquadA[biq_aB2] = biquadA[biq_aB0];
            break;
        case Highpass:
            biquadA[biq_aB0] = normv;
            biquadA[biq_aB1] = -2.0 * biquadA[biq_aB0];
            biquadA[biq_aB2] = biquadA[biq_aB0];
            break;
        case Bandpass:
            biquadA[biq_aB0] = Kv / biquadA[biq_reso] * normv;
            biquadA[biq_aB1] = 0.0;
            biquadA[biq_aB2] = -biquadA[biq_aB0];
            break;
        case Notch:
            biquadA[biq_aB0] = (1.0 + Kv * Kv) * normv;
            biquadA[biq_aB1] = 2.0 * (Kv * Kv - 1.0) * normv;
            biquadA[biq_aB2] = biquadA[biq_aB0];
            break;
    }
    biquadA[biq_bB1] = 2.0 * (Kv * Kv - 1.0) * normv;
    biquadA[biq_bB2] = (1.0 - Kv / biquadA[biq_reso] + Kv * Kv) * normv;

    // Smoothing
    mixA = mixB;
    mixB = (double)mixParam;
    inTrimA = inTrimB;
    inTrimB = pow(10.0, (double)A * 2.0 - 1.0);
    outTrimA = outTrimB;
    outTrimB = C * 10.0;
    wetA = wetB;
    wetB = pow((double)D, 2);

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

    // Helper: compute per-sample coefficients for frequency smoothing
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

    // Get channel pointers
    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);

    for (int i = 0; i < sampleFrames; ++i)
    {
        double inputSampleL = channelL[i];
        double inputSampleR = channelR[i];

        if (fabs(inputSampleL) < 1.18e-23) inputSampleL = fpdL * 1.18e-17;
        if (fabs(inputSampleR) < 1.18e-23) inputSampleR = fpdR * 1.18e-17;

        double overallDrySampleL = inputSampleL;
        double overallDrySampleR = inputSampleR;
        double drySampleL = inputSampleL;
        double drySampleR = inputSampleR;

        // Per-block coefficient interpolation factor
        double interp = (double)(sampleFrames - 1 - i) / (double)inFramesToProcess;

        double outTrim = (outTrimA * interp) + (outTrimB * (1.0 - interp));
        double wet = (wetA * interp) + (wetB * (1.0 - interp));

        double smoothedB = freqSmoothEnabled
            ? ((freqA * interp) + (freqB * (1.0 - interp)))
            : (double)B;

        double smoothedClipFactor = clipFactor;
        if (freqSmoothEnabled) {
            switch (filterType) {
                case Lowpass:  smoothedClipFactor = 1.212 - ((1.0 - smoothedB) * 0.496); break;
                case Bandpass: smoothedClipFactor = 1.0 - ((1.0 - smoothedB) * 0.304); break;
                case Notch:    smoothedClipFactor = 0.91 - ((1.0 - smoothedB) * 0.15); break;
                default: break;
            }
        }

        // Input trim + hard clip
        double inTrim = (inTrimA * interp) + (inTrimB * (1.0 - interp));
        inputSampleL *= inTrim;
        inputSampleR *= inTrim;

        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;

        double trim = freqSmoothEnabled
            ? (trimOld * interp + trimNew * (1.0 - interp))
            : trimNew;
        inputSampleL *= trim;
        inputSampleR *= trim;

        if (freqSmoothEnabled) {
            computePerSampleCoeffs(filterType, smoothedB, biquadA);
            for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
        } else {
            // Per-block biquad coefficient interpolation (original code)
            biquadA[biq_a0] = (biquadA[biq_aA0] * interp) + (biquadA[biq_aB0] * (1.0 - interp));
            biquadA[biq_a1] = (biquadA[biq_aA1] * interp) + (biquadA[biq_aB1] * (1.0 - interp));
            biquadA[biq_a2] = (biquadA[biq_aA2] * interp) + (biquadA[biq_aB2] * (1.0 - interp));
            biquadA[biq_b1] = (biquadA[biq_bA1] * interp) + (biquadA[biq_bB1] * (1.0 - interp));
            biquadA[biq_b2] = (biquadA[biq_bA2] * interp) + (biquadA[biq_bB2] * (1.0 - interp));
            // Copy coefficients to cascade stages B, C, D
            for (int x = 0; x < 7; x++) { biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x]; }
        }

        // Compute first-stage wet for overall dry blend
        double aWet = 1.0;
        { double tmpWet = wet * 4.0; if (tmpWet < 1.0) aWet = tmpWet; }

        // === processStandard: 4 cascaded biquad stages with poles crossfade ===
        {
            double bWet = 1.0, cWet = 1.0, dWet = wet * 4.0;
            if (dWet < 1.0)      { bWet = 0.0; cWet = 0.0; dWet = 0.0; }
            else if (dWet < 2.0) { bWet = dWet - 1.0; cWet = 0.0; dWet = 0.0; }
            else if (dWet < 3.0) { cWet = dWet - 2.0; dWet = 0.0; }
            else                 { dWet -= 3.0; }

            double outSmp;

            // Stage A
            if (useClip) { inputSampleL /= smoothedClipFactor; inputSampleR /= smoothedClipFactor; }
            outSmp = (inputSampleL * biquadA[biq_a0]) + biquadA[biq_sL1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            biquadA[biq_sL1] = (inputSampleL * biquadA[biq_a1]) - (outSmp * biquadA[biq_b1]) + biquadA[biq_sL2];
            biquadA[biq_sL2] = (inputSampleL * biquadA[biq_a2]) - (outSmp * biquadA[biq_b2]);
            drySampleL = inputSampleL = outSmp;

            outSmp = (inputSampleR * biquadA[biq_a0]) + biquadA[biq_sR1];
            if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
            biquadA[biq_sR1] = (inputSampleR * biquadA[biq_a1]) - (outSmp * biquadA[biq_b1]) + biquadA[biq_sR2];
            biquadA[biq_sR2] = (inputSampleR * biquadA[biq_a2]) - (outSmp * biquadA[biq_b2]);
            drySampleR = inputSampleR = outSmp;

            // Stage B
            if (bWet > 0.0) {
                if (useClip) { inputSampleL /= smoothedClipFactor; inputSampleR /= smoothedClipFactor; }
                outSmp = (inputSampleL * biquadB[biq_a0]) + biquadB[biq_sL1];
                if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
                biquadB[biq_sL1] = (inputSampleL * biquadB[biq_a1]) - (outSmp * biquadB[biq_b1]) + biquadB[biq_sL2];
                biquadB[biq_sL2] = (inputSampleL * biquadB[biq_a2]) - (outSmp * biquadB[biq_b2]);
                drySampleL = inputSampleL = (outSmp * bWet) + (drySampleL * (1.0 - bWet));

                outSmp = (inputSampleR * biquadB[biq_a0]) + biquadB[biq_sR1];
                if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
                biquadB[biq_sR1] = (inputSampleR * biquadB[biq_a1]) - (outSmp * biquadB[biq_b1]) + biquadB[biq_sR2];
                biquadB[biq_sR2] = (inputSampleR * biquadB[biq_a2]) - (outSmp * biquadB[biq_b2]);
                drySampleR = inputSampleR = (outSmp * bWet) + (drySampleR * (1.0 - bWet));
            }

            // Stage C
            if (cWet > 0.0) {
                if (useClip) { inputSampleL /= smoothedClipFactor; inputSampleR /= smoothedClipFactor; }
                outSmp = (inputSampleL * biquadC[biq_a0]) + biquadC[biq_sL1];
                if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
                biquadC[biq_sL1] = (inputSampleL * biquadC[biq_a1]) - (outSmp * biquadC[biq_b1]) + biquadC[biq_sL2];
                biquadC[biq_sL2] = (inputSampleL * biquadC[biq_a2]) - (outSmp * biquadC[biq_b2]);
                drySampleL = inputSampleL = (outSmp * cWet) + (drySampleL * (1.0 - cWet));

                outSmp = (inputSampleR * biquadC[biq_a0]) + biquadC[biq_sR1];
                if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
                biquadC[biq_sR1] = (inputSampleR * biquadC[biq_a1]) - (outSmp * biquadC[biq_b1]) + biquadC[biq_sR2];
                biquadC[biq_sR2] = (inputSampleR * biquadC[biq_a2]) - (outSmp * biquadC[biq_b2]);
                drySampleR = inputSampleR = (outSmp * cWet) + (drySampleR * (1.0 - cWet));
            }

            // Stage D
            if (dWet > 0.0) {
                if (useClip) { inputSampleL /= smoothedClipFactor; inputSampleR /= smoothedClipFactor; }
                outSmp = (inputSampleL * biquadD[biq_a0]) + biquadD[biq_sL1];
                if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
                biquadD[biq_sL1] = (inputSampleL * biquadD[biq_a1]) - (outSmp * biquadD[biq_b1]) + biquadD[biq_sL2];
                biquadD[biq_sL2] = (inputSampleL * biquadD[biq_a2]) - (outSmp * biquadD[biq_b2]);
                drySampleL = inputSampleL = (outSmp * dWet) + (drySampleL * (1.0 - dWet));

                outSmp = (inputSampleR * biquadD[biq_a0]) + biquadD[biq_sR1];
                if (outSmp > 1.0) outSmp = 1.0; if (outSmp < -1.0) outSmp = -1.0;
                biquadD[biq_sR1] = (inputSampleR * biquadD[biq_a1]) - (outSmp * biquadD[biq_b1]) + biquadD[biq_sR2];
                biquadD[biq_sR2] = (inputSampleR * biquadD[biq_a2]) - (outSmp * biquadD[biq_b2]);
                drySampleR = inputSampleR = (outSmp * dWet) + (drySampleR * (1.0 - dWet));
            }

            if (useClip) { inputSampleL /= smoothedClipFactor; inputSampleR /= smoothedClipFactor; }
        }

        // === processOpamp: DC block + 2x lowpass + soft clip + output gain ===
        {
            double outSmp;

            if (fabs(iirSampleAL) < 1.18e-37) iirSampleAL = 0.0;
            iirSampleAL = (iirSampleAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
            inputSampleL -= iirSampleAL;
            if (fabs(iirSampleAR) < 1.18e-37) iirSampleAR = 0.0;
            iirSampleAR = (iirSampleAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
            inputSampleR -= iirSampleAR;

            outSmp = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSmp * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSmp * fixA[fix_b2]);
            inputSampleL = outSmp;
            outSmp = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSmp * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSmp * fixA[fix_b2]);
            inputSampleR = outSmp;

            if (inputSampleL > 1.0) inputSampleL = 1.0; if (inputSampleL < -1.0) inputSampleL = -1.0;
            inputSampleL -= (inputSampleL * inputSampleL * inputSampleL * inputSampleL * inputSampleL * 0.1768);
            if (inputSampleR > 1.0) inputSampleR = 1.0; if (inputSampleR < -1.0) inputSampleR = -1.0;
            inputSampleR -= (inputSampleR * inputSampleR * inputSampleR * inputSampleR * inputSampleR * 0.1768);

            outSmp = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
            fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSmp * fixB[fix_b1]) + fixB[fix_sL2];
            fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSmp * fixB[fix_b2]);
            inputSampleL = outSmp;
            outSmp = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
            fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSmp * fixB[fix_b1]) + fixB[fix_sR2];
            fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSmp * fixB[fix_b2]);
            inputSampleR = outSmp;

            if (outTrim != 1.0) {
                inputSampleL *= outTrim;
                inputSampleR *= outTrim;
            }
        }

        // Poles-based dry blend
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
bool ZFilterMiniProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* ZFilterMiniProcessor::createEditor()
{
    return new ZFilterMiniEditor(*this);
}

//==============================================================================
void ZFilterMiniProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ZFilterMiniProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZFilterMiniProcessor();
}
