# Z-Filter Mini

A streamlined single-knob filter plugin built with JUCE, based on the Airwindows Z-series filters by Chris Johnson. Minimal controls, maximum tone.

![Z-Filter Mini GUI](GUI/screenshot-mini.png)

## Download

| Version | Description | Download |
|---------|-------------|----------|
| **Z-Filter Mini** | Single filter, no LFO, minimal UI | **[Z-FilterMini.vst3 (Win x64)](https://github.com/sleepingsw0rd/Z-Filter/raw/mini/Releases/Z-FilterMini.vst3/Contents/x86_64-win/Z-FilterMini.vst3)** |
| **Z-Filter** | Single filter, single LFO, compact UI | **[Z-Filter.vst3 (Win x64)](https://github.com/sleepingsw0rd/Z-Filter/raw/main/Releases/Z-Filter.vst3/Contents/x86_64-win/Z-Filter.vst3)** |
| **Z-Filter XL** | Dual filter, dual LFO, morph with LFO, expanded UI | **[Z-FilterXL.vst3 (Win x64)](https://github.com/sleepingsw0rd/Z-Filter/raw/v2-xl/Releases/Z-FilterXL.vst3/Contents/x86_64-win/Z-FilterXL.vst3)** |

Drop into your VST3 folder (typically `C:\Program Files\Common Files\VST3\`) and scan in your DAW. All three versions can run side-by-side.

## Features

- **4 Filter Types**: Lowpass, Highpass, Bandpass, Notch
- **Cascaded Biquad Architecture**: Up to 4 cascaded filter stages (12-48 dB/octave) controlled by the Poles knob
- **Large Frequency Knob**: Prominent cutoff control front and center
- **Input & Output Level Controls**: Input gain drives the filter's saturation character; Output gain for final level adjustment
- **Opamp Modeling Stage**: Bandwidth-limited analog op-amp simulation with polynomial soft saturation
- **Dot-Matrix LCD Display**: Real-time parameter readout in an HD44780-style display
- **Dry/Wet Mix**: Blend filtered and dry signals
- **Per-Sample Coefficient Smoothing**: Click-free parameter automation
- **Tiny Footprint**: 723x342 pixel window — fits anywhere

## Controls

| Control | Function |
|---------|----------|
| **Frequency** (large knob) | Filter cutoff frequency |
| **LP / HP / BP / NT** | Filter type selector buttons with LED indicators |
| **Input** | Pre-filter input gain |
| **Poles** | Filter resonance / cascade depth (1-4 stages) |
| **Level** | Post-filter output gain |
| **Mix** | Dry/wet blend |

## Building

### Requirements

- CMake 3.22+
- C++17 compatible compiler (MSVC, GCC, Clang)
- JUCE 7.0.12 (fetched automatically via CMake)

### Build Steps

```bash
cmake -B build -S .
cmake --build build --config Release
```

Build artifacts:

- **VST3**: `build/ZFilterMini_artefacts/Release/VST3/Z-FilterMini.vst3`
- **Standalone**: `build/ZFilterMini_artefacts/Release/Standalone/Z-FilterMini.exe`

## DSP Signal Flow

1. Input gain with hard clip to [-1, 1]
2. Frequency-dependent trim scaling
3. Up to 4 cascaded biquad filter stages (Transposed Direct Form II) with inter-stage clipping
4. DC-blocking highpass (~5 Hz)
5. Opamp stage: 15.5 kHz lowpass → polynomial soft saturation → 15.5 kHz lowpass
6. Output gain
7. Wet/dry mix blend
8. TPDF dither

## Credits

- **Filter algorithms**: Based on [Airwindows](https://www.airwindows.com/) ZLowpass2, ZHighpass2, ZBandpass2, ZNotch2 by Chris Johnson (MIT License)
- **Framework**: [JUCE](https://juce.com/) 7.0.12

## License

Filter DSP algorithms are derived from Airwindows code, licensed under the MIT License. See `Source/AirwindowsReference/FilterReference.h` for details.
