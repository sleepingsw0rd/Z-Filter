#include "PluginEditor.h"

//==============================================================================
// 5x7 dot-matrix font data (ASCII 32-127)
// Each character is 5 bytes, one per column, LSB = top row
//==============================================================================
const uint8_t DotMatrixLCD::font5x7[] = {
    // 32 SPACE
    0x00, 0x00, 0x00, 0x00, 0x00,
    // 33 !
    0x00, 0x00, 0x5F, 0x00, 0x00,
    // 34 "
    0x00, 0x07, 0x00, 0x07, 0x00,
    // 35 #
    0x14, 0x7F, 0x14, 0x7F, 0x14,
    // 36 $
    0x24, 0x2A, 0x7F, 0x2A, 0x12,
    // 37 %
    0x23, 0x13, 0x08, 0x64, 0x62,
    // 38 &
    0x36, 0x49, 0x55, 0x22, 0x50,
    // 39 '
    0x00, 0x05, 0x03, 0x00, 0x00,
    // 40 (
    0x00, 0x1C, 0x22, 0x41, 0x00,
    // 41 )
    0x00, 0x41, 0x22, 0x1C, 0x00,
    // 42 *
    0x14, 0x08, 0x3E, 0x08, 0x14,
    // 43 +
    0x08, 0x08, 0x3E, 0x08, 0x08,
    // 44 ,
    0x00, 0x50, 0x30, 0x00, 0x00,
    // 45 -
    0x08, 0x08, 0x08, 0x08, 0x08,
    // 46 .
    0x00, 0x60, 0x60, 0x00, 0x00,
    // 47 /
    0x20, 0x10, 0x08, 0x04, 0x02,
    // 48 0
    0x3E, 0x51, 0x49, 0x45, 0x3E,
    // 49 1
    0x00, 0x42, 0x7F, 0x40, 0x00,
    // 50 2
    0x42, 0x61, 0x51, 0x49, 0x46,
    // 51 3
    0x21, 0x41, 0x45, 0x4B, 0x31,
    // 52 4
    0x18, 0x14, 0x12, 0x7F, 0x10,
    // 53 5
    0x27, 0x45, 0x45, 0x45, 0x39,
    // 54 6
    0x3C, 0x4A, 0x49, 0x49, 0x30,
    // 55 7
    0x01, 0x71, 0x09, 0x05, 0x03,
    // 56 8
    0x36, 0x49, 0x49, 0x49, 0x36,
    // 57 9
    0x06, 0x49, 0x49, 0x29, 0x1E,
    // 58 :
    0x00, 0x36, 0x36, 0x00, 0x00,
    // 59 ;
    0x00, 0x56, 0x36, 0x00, 0x00,
    // 60 <
    0x08, 0x14, 0x22, 0x41, 0x00,
    // 61 =
    0x14, 0x14, 0x14, 0x14, 0x14,
    // 62 >
    0x00, 0x41, 0x22, 0x14, 0x08,
    // 63 ?
    0x02, 0x01, 0x51, 0x09, 0x06,
    // 64 @
    0x32, 0x49, 0x79, 0x41, 0x3E,
    // 65 A
    0x7E, 0x11, 0x11, 0x11, 0x7E,
    // 66 B
    0x7F, 0x49, 0x49, 0x49, 0x36,
    // 67 C
    0x3E, 0x41, 0x41, 0x41, 0x22,
    // 68 D
    0x7F, 0x41, 0x41, 0x22, 0x1C,
    // 69 E
    0x7F, 0x49, 0x49, 0x49, 0x41,
    // 70 F
    0x7F, 0x09, 0x09, 0x09, 0x01,
    // 71 G
    0x3E, 0x41, 0x49, 0x49, 0x7A,
    // 72 H
    0x7F, 0x08, 0x08, 0x08, 0x7F,
    // 73 I
    0x00, 0x41, 0x7F, 0x41, 0x00,
    // 74 J
    0x20, 0x40, 0x41, 0x3F, 0x01,
    // 75 K
    0x7F, 0x08, 0x14, 0x22, 0x41,
    // 76 L
    0x7F, 0x40, 0x40, 0x40, 0x40,
    // 77 M
    0x7F, 0x02, 0x0C, 0x02, 0x7F,
    // 78 N
    0x7F, 0x04, 0x08, 0x10, 0x7F,
    // 79 O
    0x3E, 0x41, 0x41, 0x41, 0x3E,
    // 80 P
    0x7F, 0x09, 0x09, 0x09, 0x06,
    // 81 Q
    0x3E, 0x41, 0x51, 0x21, 0x5E,
    // 82 R
    0x7F, 0x09, 0x19, 0x29, 0x46,
    // 83 S
    0x46, 0x49, 0x49, 0x49, 0x31,
    // 84 T
    0x01, 0x01, 0x7F, 0x01, 0x01,
    // 85 U
    0x3F, 0x40, 0x40, 0x40, 0x3F,
    // 86 V
    0x1F, 0x20, 0x40, 0x20, 0x1F,
    // 87 W
    0x3F, 0x40, 0x38, 0x40, 0x3F,
    // 88 X
    0x63, 0x14, 0x08, 0x14, 0x63,
    // 89 Y
    0x07, 0x08, 0x70, 0x08, 0x07,
    // 90 Z
    0x61, 0x51, 0x49, 0x45, 0x43,
    // 91 [
    0x00, 0x7F, 0x41, 0x41, 0x00,
    // 92 backslash
    0x02, 0x04, 0x08, 0x10, 0x20,
    // 93 ]
    0x00, 0x41, 0x41, 0x7F, 0x00,
    // 94 ^
    0x04, 0x02, 0x01, 0x02, 0x04,
    // 95 _
    0x40, 0x40, 0x40, 0x40, 0x40,
    // 96 `
    0x00, 0x01, 0x02, 0x04, 0x00,
    // 97 a
    0x20, 0x54, 0x54, 0x54, 0x78,
    // 98 b
    0x7F, 0x48, 0x44, 0x44, 0x38,
    // 99 c
    0x38, 0x44, 0x44, 0x44, 0x20,
    // 100 d
    0x38, 0x44, 0x44, 0x48, 0x7F,
    // 101 e
    0x38, 0x54, 0x54, 0x54, 0x18,
    // 102 f
    0x08, 0x7E, 0x09, 0x01, 0x02,
    // 103 g
    0x0C, 0x52, 0x52, 0x52, 0x3E,
    // 104 h
    0x7F, 0x08, 0x04, 0x04, 0x78,
    // 105 i
    0x00, 0x44, 0x7D, 0x40, 0x00,
    // 106 j
    0x20, 0x40, 0x44, 0x3D, 0x00,
    // 107 k
    0x7F, 0x10, 0x28, 0x44, 0x00,
    // 108 l
    0x00, 0x41, 0x7F, 0x40, 0x00,
    // 109 m
    0x7C, 0x04, 0x18, 0x04, 0x78,
    // 110 n
    0x7C, 0x08, 0x04, 0x04, 0x78,
    // 111 o
    0x38, 0x44, 0x44, 0x44, 0x38,
    // 112 p
    0x7C, 0x14, 0x14, 0x14, 0x08,
    // 113 q
    0x08, 0x14, 0x14, 0x18, 0x7C,
    // 114 r
    0x7C, 0x08, 0x04, 0x04, 0x08,
    // 115 s
    0x48, 0x54, 0x54, 0x54, 0x20,
    // 116 t
    0x04, 0x3F, 0x44, 0x40, 0x20,
    // 117 u
    0x3C, 0x40, 0x40, 0x20, 0x7C,
    // 118 v
    0x1C, 0x20, 0x40, 0x20, 0x1C,
    // 119 w
    0x3C, 0x40, 0x30, 0x40, 0x3C,
    // 120 x
    0x44, 0x28, 0x10, 0x28, 0x44,
    // 121 y
    0x0C, 0x50, 0x50, 0x50, 0x3C,
    // 122 z
    0x44, 0x64, 0x54, 0x4C, 0x44,
    // 123 {
    0x00, 0x08, 0x36, 0x41, 0x00,
    // 124 |
    0x00, 0x00, 0x7F, 0x00, 0x00,
    // 125 }
    0x00, 0x41, 0x36, 0x08, 0x00,
    // 126 ~
    0x10, 0x08, 0x08, 0x10, 0x08,
    // 127 DEL
    0x00, 0x00, 0x00, 0x00, 0x00,
};

DotMatrixLCD::DotMatrixLCD()
{
    setOpaque(false);
}

void DotMatrixLCD::setText(int row, const juce::String& text)
{
    if (row >= 0 && row < kRows && rows[row] != text)
    {
        rows[row] = text;
        dirty = true;
        repaint();
    }
}

void DotMatrixLCD::drawChar(juce::Graphics& g, char ch, float x, float y, float dotSize, float dotPitch) const
{
    int idx = (int)ch - 32;
    if (idx < 0 || idx > 95) idx = 0; // fallback to space

    const uint8_t* glyph = &font5x7[idx * 5];

    for (int col = 0; col < kCharW; col++)
    {
        uint8_t colData = glyph[col];
        for (int row = 0; row < kCharH; row++)
        {
            float dx = x + col * dotPitch;
            float dy = y + row * dotPitch;

            if (colData & (1 << row))
            {
                // Active dot - dark green/black
                g.setColour(juce::Colour(0xff0a2a0a));
                g.fillEllipse(dx, dy, dotSize, dotSize);
            }
            else
            {
                // Ghost dot - very faint
                g.setColour(juce::Colour(0x15000000));
                g.fillEllipse(dx, dy, dotSize, dotSize);
            }
        }
    }
}

void DotMatrixLCD::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Calculate dot sizing to fit within bounds
    float totalDotsW = (float)(kCols * (kCharW + 1) - 1);
    float totalDotsH = (float)(kRows * (kCharH + 2) - 2);
    float dotPitch = juce::jmin(bounds.getWidth() / (totalDotsW + 2.0f),
                                 bounds.getHeight() / (totalDotsH + 2.0f));
    float dotSize = dotPitch * 0.75f;

    float charPitchX = (kCharW + 1) * dotPitch;
    float charPitchY = (kCharH + 2) * dotPitch;

    // Center the display
    float totalW = kCols * charPitchX - dotPitch;
    float totalH = kRows * charPitchY - 2 * dotPitch;
    float startX = bounds.getX() + (bounds.getWidth() - totalW) / 2.0f;
    float startY = bounds.getY() + (bounds.getHeight() - totalH) / 2.0f;

    for (int row = 0; row < kRows; row++)
    {
        auto text = rows[row];
        for (int col = 0; col < kCols; col++)
        {
            char ch = (col < text.length()) ? (char)text[col] : ' ';
            float cx = startX + col * charPitchX;
            float cy = startY + row * charPitchY;
            drawChar(g, ch, cx, cy, dotSize, dotPitch);
        }
    }

    dirty = false;
}

//==============================================================================
// Editor
//==============================================================================
ZFilterEditor::ZFilterEditor(ZFilterProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1200, 344);

    // Load background image (1200px wide version)
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::GUINoControlsNoBigKnob1200_png, BinaryData::GUINoControlsNoBigKnob1200_pngSize);

    // LCD
    addAndMakeVisible(lcd);

    // Knobs
    addAndMakeVisible(frequencyKnob);
    addAndMakeVisible(resonanceKnob);
    addAndMakeVisible(outputKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(inputKnob);
    addAndMakeVisible(lfoSpeedKnob);
    addAndMakeVisible(lfoDepthKnob);
    addAndMakeVisible(morphKnob);

    // Buttons
    auto addBtn = [this](RoundButtonComponent& btn) { addAndMakeVisible(btn); };
    addBtn(bypassBtn);
    addBtn(lpBtn);
    addBtn(hpBtn);
    addBtn(bpBtn);
    addBtn(ntBtn);
    addBtn(rgBtn);
    addBtn(lfoSyncBtn);
    addBtn(zOutBtn);
    addBtn(morphEnableBtn);
    addBtn(filterABtn);
    addBtn(filterBBtn);
    addBtn(lfoTargetBtn);

    // LEDs
    addAndMakeVisible(bypassLED);
    addAndMakeVisible(lpLED);
    addAndMakeVisible(hpLED);
    addAndMakeVisible(bpLED);
    addAndMakeVisible(ntLED);
    addAndMakeVisible(rgLED);
    addAndMakeVisible(lfoSyncLED);
    addAndMakeVisible(zOutLED);
    addAndMakeVisible(morphEnableLED);
    addAndMakeVisible(filterALED);
    addAndMakeVisible(filterBLED);
    addAndMakeVisible(lfoTargetLED);

    // Button callbacks
    bypassBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("bypass");
        param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
    };

    auto setFilter = [this](int index) {
        processorRef.apvts.getParameter("filterTypeA")->setValueNotifyingHost((float)index / 4.0f);
    };
    lpBtn.onClick = [setFilter]() { setFilter(0); };
    hpBtn.onClick = [setFilter]() { setFilter(1); };
    bpBtn.onClick = [setFilter]() { setFilter(2); };
    ntBtn.onClick = [setFilter]() { setFilter(3); };
    rgBtn.onClick = [setFilter]() { setFilter(4); };

    zOutBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("zOutputStage");
        param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
    };

    lfoSyncBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("lfoSync");
        param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
    };

    // Morph enable toggle
    morphEnableBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("morphEnabled");
        param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
    };

    // Filter A stepped selector (cycles through LP/HP/BP/NT/RG)
    filterABtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("filterTypeA");
        int current = (int)(param->getValue() * 4.0f + 0.5f);
        int next = (current + 1) % 5;
        param->setValueNotifyingHost((float)next / 4.0f);
    };

    // Filter B stepped selector
    filterBBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("filterTypeB");
        int current = (int)(param->getValue() * 4.0f + 0.5f);
        int next = (current + 1) % 5;
        param->setValueNotifyingHost((float)next / 4.0f);
    };

    // LFO target 3-way toggle (Cutoff/Morph/Both)
    lfoTargetBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("lfoTarget");
        int current = (int)(param->getValue() * 2.0f + 0.5f);
        int next = (current + 1) % 3;
        param->setValueNotifyingHost((float)next / 2.0f);
    };

    // Parameter attachments
    frequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "frequency", frequencyKnob);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "resonance", resonanceKnob);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "output", outputKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "mix", mixKnob);
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "input", inputKnob);
    lfoSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "lfoSpeed", lfoSpeedKnob);
    lfoDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "lfoDepth", lfoDepthKnob);
    morphAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "morph", morphKnob);

    updateDisplay();
    startTimerHz(15);
}

ZFilterEditor::~ZFilterEditor()
{
    stopTimer();
}

void ZFilterEditor::timerCallback()
{
    updateDisplay();
}

void ZFilterEditor::updateDisplay()
{
    // Read params
    int filterTypeA = (int)*processorRef.apvts.getRawParameterValue("filterTypeA");
    int filterTypeB = (int)*processorRef.apvts.getRawParameterValue("filterTypeB");
    bool morphOn = *processorRef.apvts.getRawParameterValue("morphEnabled") > 0.5f;
    float morph = *processorRef.apvts.getRawParameterValue("morph");
    int lfoTgt = (int)*processorRef.apvts.getRawParameterValue("lfoTarget");
    float freq = *processorRef.apvts.getRawParameterValue("frequency");
    float reso = *processorRef.apvts.getRawParameterValue("resonance");
    float output = *processorRef.apvts.getRawParameterValue("output");
    float mix = *processorRef.apvts.getRawParameterValue("mix");
    float input = *processorRef.apvts.getRawParameterValue("input");
    bool bypassed = *processorRef.apvts.getRawParameterValue("bypass") > 0.5f;
    bool zOut = *processorRef.apvts.getRawParameterValue("zOutputStage") > 0.5f;
    float lfoSpd = *processorRef.apvts.getRawParameterValue("lfoSpeed");
    float lfoDpt = *processorRef.apvts.getRawParameterValue("lfoDepth");
    bool lfoSync = *processorRef.apvts.getRawParameterValue("lfoSync") > 0.5f;

    // Filter type short names
    const char* typeShort[] = { "LP", "HP", "BP", "NT", "RG" };
    const char* typeNames[] = { "LOWPASS", "HIGHPASS", "BANDPASS", "NOTCH", "REGION" };
    const char* tgtNames[] = { "CUT", "MRP", "C+M" };

    juce::String row0, row1, row2, row3;

    if (morphOn) {
        juce::String tA = (filterTypeA >= 0 && filterTypeA <= 4) ? typeShort[filterTypeA] : "LP";
        juce::String tB = (filterTypeB >= 0 && filterTypeB <= 4) ? typeShort[filterTypeB] : "BP";
        row0 = "Z-FILTER MORPH " + tA + ">" + tB + (zOut ? " ZOUT" : "");
        row2 = "MORPH:" + juce::String(morph * 100.0f, 0) + "% RES:" + juce::String(reso * 100.0f, 0) + "%";
    } else {
        juce::String typeName = (filterTypeA >= 0 && filterTypeA <= 4) ? typeNames[filterTypeA] : "LOWPASS";
        row0 = "Z-FILTER " + typeName + (zOut ? " ZOUT" : "");
        row2 = "IN:" + juce::String(input * 100.0f, 0) + "% RES:" + juce::String(reso * 100.0f, 0) + "%";
    }

    // Row 1: Frequency + LFO info
    if (lfoDpt > 0.001f) {
        juce::String lfoInfo;
        if (lfoSync) {
            static const char* divNames[] = {
                "4/1","4/1D","4/1T","2/1","2/1D","2/1T",
                "1/1","1/1D","1/1T","1/2","1/2D","1/2T",
                "1/4","1/4D","1/4T","1/8","1/8D","1/8T",
                "1/16","1/16D","1/16T","1/32","1/32D","1/32T",
                "1/64","1/64D","1/64T"
            };
            int divIdx = juce::jlimit(0, 26, (int)(lfoSpd * 26.0f + 0.5f));
            lfoInfo = divNames[divIdx];
        } else {
            double hz = 0.01 * pow(2000.0, (double)lfoSpd);
            lfoInfo = juce::String(hz, 1) + "Hz";
        }
        juce::String tgt = tgtNames[juce::jlimit(0, 2, lfoTgt)];
        row1 = "FRQ:" + juce::String(freq * 100.0f, 0) + "% LFO:" + lfoInfo + " " + tgt;
    } else {
        row1 = "FREQ: " + juce::String(freq * 100.0f, 1) + "%";
    }

    row3 = "LVL:" + juce::String(output * 100.0f, 0) + "% MIX:" + juce::String(mix * 100.0f, 0) + "%" + (bypassed ? " BYP" : "");

    lcd.setText(0, row0);
    lcd.setText(1, row1);
    lcd.setText(2, row2);
    lcd.setText(3, row3);

    // LEDs
    bypassLED.setActive(bypassed);
    lpLED.setActive(filterTypeA == 0);
    hpLED.setActive(filterTypeA == 1);
    bpLED.setActive(filterTypeA == 2);
    ntLED.setActive(filterTypeA == 3);
    rgLED.setActive(filterTypeA == 4);
    lfoSyncLED.setActive(lfoSync);
    zOutLED.setActive(zOut);
    morphEnableLED.setActive(morphOn);
    filterALED.setActive(morphOn);
    filterBLED.setActive(morphOn);
    lfoTargetLED.setActive(lfoTgt > 0);

    repaint();
}

//==============================================================================
void ZFilterEditor::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
        g.drawImageAt(backgroundImage, 0, 0);
    else
        g.fillAll(juce::Colour(0xff1a1a1a));

    // Draw text labels for all controls
    // Layout (19 controls) at 57px center-to-center spacing, starting at x=32
    g.setColour(juce::Colour(0xffb0b0b0));
    g.setFont(juce::Font(11.0f));

    // Dynamic labels for Flt A, Flt B, and Tgt
    const char* typeShort[] = { "LP", "HP", "BP", "NT", "RG" };
    const char* tgtNames[] = { "CUT", "MRP", "C+M" };
    int filterTypeA = (int)*processorRef.apvts.getRawParameterValue("filterTypeA");
    int filterTypeB = (int)*processorRef.apvts.getRawParameterValue("filterTypeB");
    int lfoTgt = (int)*processorRef.apvts.getRawParameterValue("lfoTarget");

    juce::String fltALabel = "A>" + juce::String(typeShort[juce::jlimit(0, 4, filterTypeA)]);
    juce::String fltBLabel = "B>" + juce::String(typeShort[juce::jlimit(0, 4, filterTypeB)]);
    juce::String tgtLabel = juce::String("Tgt>") + tgtNames[juce::jlimit(0, 2, lfoTgt)];

    const int labelY = 253;
    g.drawText("Bypass",   57, labelY, 50, 14, juce::Justification::centred);
    g.drawText("ZOut",    114, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Morph",   171, labelY, 50, 14, juce::Justification::centred);
    g.drawText(fltALabel,  228, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Mrph",    285, labelY, 50, 14, juce::Justification::centred);
    g.drawText(fltBLabel,  342, labelY, 50, 14, juce::Justification::centred);
    g.drawText("LP",      399, labelY, 50, 14, juce::Justification::centred);
    g.drawText("HP",      456, labelY, 50, 14, juce::Justification::centred);
    g.drawText("BP",      513, labelY, 50, 14, juce::Justification::centred);
    g.drawText("NT",      570, labelY, 50, 14, juce::Justification::centred);
    g.drawText("RG",      627, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Spd",     684, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Dpt",     741, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Sync",    798, labelY, 50, 14, juce::Justification::centred);
    g.drawText(tgtLabel,   855, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Input",   912, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Res",     969, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Level",  1026, labelY, 50, 14, juce::Justification::centred);
    g.drawText("Mix",    1083, labelY, 50, 14, juce::Justification::centred);
}

void ZFilterEditor::resized()
{
    // LCD display - wider for 1200px layout
    lcd.setBounds(115, 62, 580, 110);

    // Frequency knob (large) - pushed right for wider layout
    frequencyKnob.setBounds(900, 18, 188, 188);

    // Control strip layout for 1200px width
    // 19 controls at 57px center-to-center spacing, starting at center=82
    // Centers: 82, 139, 196, 253, 310, 367, 424, 481, 538, 595, 652, 709, 766, 823, 880, 937, 994, 1051, 1108
    const int ledY  = 241;
    const int btnY  = 274;
    const int knobY = 267;

    // Bypass (center=82)
    bypassBtn.setBounds(62, btnY, 40, 24);
    bypassLED.setBounds(77, ledY, 10, 10);

    // ZOut (center=139)
    zOutBtn.setBounds(119, btnY, 40, 24);
    zOutLED.setBounds(134, ledY, 10, 10);

    // Morph Enable (center=196)
    morphEnableBtn.setBounds(176, btnY, 40, 24);
    morphEnableLED.setBounds(191, ledY, 10, 10);

    // Filter A stepped selector (center=253)
    filterABtn.setBounds(233, btnY, 40, 24);
    filterALED.setBounds(248, ledY, 10, 10);

    // Morph knob (center=310)
    morphKnob.setBounds(291, knobY, 38, 38);

    // Filter B stepped selector (center=367)
    filterBBtn.setBounds(347, btnY, 40, 24);
    filterBLED.setBounds(362, ledY, 10, 10);

    // Filter type buttons (LP/HP/BP/NT/RG) - quick-set for Filter A
    lpBtn.setBounds(404, btnY, 40, 24);
    hpBtn.setBounds(461, btnY, 40, 24);
    bpBtn.setBounds(518, btnY, 40, 24);
    ntBtn.setBounds(575, btnY, 40, 24);
    rgBtn.setBounds(632, btnY, 40, 24);

    lpLED.setBounds(419, ledY, 10, 10);
    hpLED.setBounds(476, ledY, 10, 10);
    bpLED.setBounds(533, ledY, 10, 10);
    ntLED.setBounds(590, ledY, 10, 10);
    rgLED.setBounds(647, ledY, 10, 10);

    // LFO controls
    lfoSpeedKnob.setBounds(690, knobY, 38, 38);
    lfoDepthKnob.setBounds(747, knobY, 38, 38);
    lfoSyncBtn.setBounds(803, btnY, 40, 24);
    lfoSyncLED.setBounds(818, ledY, 10, 10);

    // LFO Target (center=880)
    lfoTargetBtn.setBounds(860, btnY, 40, 24);
    lfoTargetLED.setBounds(875, ledY, 10, 10);

    // Parameter knobs
    inputKnob.setBounds(918, knobY, 38, 38);
    resonanceKnob.setBounds(975, knobY, 38, 38);
    outputKnob.setBounds(1032, knobY, 38, 38);
    mixKnob.setBounds(1089, knobY, 38, 38);
}
