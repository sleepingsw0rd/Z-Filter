# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Z-Filter is a biquad filter VST3/Standalone audio plugin family built with JUCE 7.0.12 and C++17. The DSP algorithms are derived from the Airwindows Z-series filters by Chris Johnson (MIT License). Produced by "SleepingSword".

Three variants live on separate git branches, each with its own PRODUCT_NAME, plugin code, and feature set:

| Variant | Branch | Product Name | Features |
|---------|--------|--------------|----------|
| **Z-Filter Mini** | `mini` | Z-FilterMini | Single filter, no LFO, minimal UI |
| **Z-Filter** | `main` | Z-Filter | Single filter, single LFO, compact UI |
| **Z-Filter XL** | `v2-xl` | Z-FilterXL | Dual filter, dual LFO, morph, expanded UI |

All three versions can be installed side-by-side. Each branch has its own README with version-specific documentation.

## Build Commands

```bash
cmake -B build -S .
cmake --build build --config Release
```

Requirements: CMake 3.22+, C++17 compiler (MSVC/GCC/Clang). JUCE 7.0.12 is fetched automatically via CMake FetchContent.

Build artifacts per variant (product name changes per branch):

| Variant | VST3 artifact | Standalone artifact |
|---------|---------------|---------------------|
| **Mini** (`mini`) | `build/ZFilter_artefacts/Release/VST3/Z-FilterMini.vst3` | `build/ZFilter_artefacts/Release/Standalone/Z-FilterMini.exe` |
| **Main** (`main`) | `build/ZFilter_artefacts/Release/VST3/Z-Filter.vst3` | `build/ZFilter_artefacts/Release/Standalone/Z-Filter.exe` |
| **XL** (`v2-xl`) | `build/ZFilter_artefacts/Release/VST3/Z-FilterXL.vst3` | `build/ZFilter_artefacts/Release/Standalone/Z-FilterXL.exe` |

Pre-built VST3 binaries are committed under `Releases/` on each branch.

There are no automated tests. Testing is done manually by loading the VST3 in a DAW.

## Architecture

The entire plugin is four files:

| File | Role |
|------|------|
| `Source/PluginProcessor.h` | Processor class, biquad state arrays, enums |
| `Source/PluginProcessor.cpp` | All DSP: parameter layout, filter coefficients, LFOs, process block |
| `Source/PluginEditor.h` | Custom GUI component classes (KnobComponent, LEDComponent, RoundButtonComponent, DotMatrixLCD) |
| `Source/PluginEditor.cpp` | GUI layout, embedded 5x7 dot-matrix font data, display update logic, paint/resized |

`Source/AirwindowsReference/` contains original Airwindows algorithm documentation and earlier plugin versions for reference only — not compiled.

`GUI/` contains PNG background images and screenshots. Two images are embedded as binary data via `juce_add_binary_data` in CMakeLists.txt.

## DSP Architecture

All variants share the same core DSP (biquad filters, opamp stage, TPDF dither). The variants differ in how many filters and modulation sources are exposed:
- **Mini**: Single filter, no LFO, no morph
- **Main**: Single filter, single LFO, no morph
- **XL**: Dual filters (A+B), dual LFOs (A+B), morph with dedicated LFO, serial/parallel routing

**Signal flow** (per-sample, double-precision internal, float I/O):

1. Input gain → hard clip [-1, 1] → frequency-dependent trim
2. LFO A/B modulate cutoff frequency; Morph LFO modulates morph position (XL only)
3. Filter processing (XL has four routing paths; Mini/Main use single filter only):
   - **Morph mode**: Coefficient interpolation between Filter A and B biquads
   - **Parallel**: Both filters process dry input, outputs averaged
   - **Serial**: Filter A output → Filter B input
   - **Single**: Only the enabled filter processes
4. Each filter: up to 5 cascaded biquad stages (Transposed Direct Form II), controlled by Poles parameter
5. Opamp stage (optional "Z-Output"): two 15.5 kHz lowpass → polynomial soft saturation between them
6. Output gain → wet/dry mix → TPDF dither

**Key DSP details:**
- Filter types: Lowpass (Q=0.7071), Highpass (Q=1.0), Bandpass (Q=0.314), Notch (Q=0.618)
- Biquad state arrays use Airwindows indexing convention: `biq_freq` through `biq_sR2` (21 elements per stage)
- `biquadA`–`biquadE` = Filter A's 5 cascade stages; `biquadA2`–`biquadE2` = Filter B
- `fixA`/`fixB` = opamp stage biquad state
- LFOs are sine-wave with 27 tempo-sync divisions (4/1 down to 1/64 with dotted/triplet)
- `freqSmooth` mode recalculates filter coefficients every sample for click-free automation (CPU-intensive)
- All parameter changes interpolate within audio blocks to avoid clicks

## GUI Architecture

Custom-drawn JUCE components (no stock LookAndFeel), 1200x450 pixel fixed size:

- **KnobComponent**: Rotary slider with metallic gradient rendering (two sizes: large for frequency, small for all others)
- **RoundButtonComponent**: Toggle buttons with 3D cap, drop shadow, pressed animation
- **LEDComponent**: 10x10 circular indicators (red glow when active)
- **DotMatrixLCD**: 4-row x 29-char display using embedded 5x7 pixel font (HD44780 style)

Timer callback at 15 Hz reads all APVTS parameters and updates LEDs + LCD text. All parameters are managed through `juce::AudioProcessorValueTreeState` (APVTS) with slider/button attachments.

## Parameter IDs

Parameters are accessed via string IDs in the APVTS. Key IDs follow these patterns:
- Filter: `filterAType`, `filterBType`, `filterAEnabled`, `filterBEnabled`, `polesA`, `polesB`, `routing`
- LFO: `lfoASpeed`, `lfoADepth`, `lfoASync`, `lfoBSpeed`, `lfoBDepth`, `lfoBSync`, `lfoLink`
- Morph: `morphEnabled`, `morphPosition`, `morphLfoSpeed`, `morphLfoDepth`, `morphLfoSync`
- Master: `frequency`, `inputGain`, `outputGain`, `mix`, `bypass`, `zOutputStage`, `freqSmooth`

## Conventions

- DSP runs in double precision internally; I/O is 32-bit float
- Denormal threshold: 1.18e-23
- The TPDF dither uses `fpdL`/`fpdR` uint32 state (Airwindows convention)
- Filter coefficient computation is in a dedicated section (~lines 224-292 of PluginProcessor.cpp) with type-specific frequency power curves and clipping factors
- GUI coordinates are hardcoded pixel positions in `resized()` — there is no responsive layout
