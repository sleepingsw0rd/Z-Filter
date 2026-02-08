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
    setSize(1200, 450);

    // Load background image (1200x450 version)
    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::GUIWithLogo1200BigUI_png, BinaryData::GUIWithLogo1200BigUI_pngSize);

    // LCD
    addAndMakeVisible(lcd);

    // Helper to add components
    auto addKnob = [this](KnobComponent& k) { addAndMakeVisible(k); };
    auto addBtn  = [this](RoundButtonComponent& b) { addAndMakeVisible(b); };
    auto addLED  = [this](LEDComponent& l) { addAndMakeVisible(l); };

    // Frequency knob
    addKnob(frequencyKnob);

    // Filter A row
    addBtn(filterAEnableBtn); addLED(filterAEnableLED);
    addBtn(lpABtn); addBtn(hpABtn); addBtn(bpABtn); addBtn(ntABtn); addBtn(rgABtn);
    addLED(lpALED); addLED(hpALED); addLED(bpALED); addLED(ntALED); addLED(rgALED);
    addKnob(polesAKnob);

    // Filter B row
    addBtn(filterBEnableBtn); addLED(filterBEnableLED);
    addBtn(lpBBtn); addBtn(hpBBtn); addBtn(bpBBtn); addBtn(ntBBtn); addBtn(rgBBtn);
    addLED(lpBLED); addLED(hpBLED); addLED(bpBLED); addLED(ntBLED); addLED(rgBLED);
    addKnob(polesBKnob);

    // Routing
    addBtn(routingBtn); addLED(routingLED);

    // LFO A
    addKnob(lfoASpeedKnob); addKnob(lfoADepthKnob);
    addBtn(lfoASyncBtn); addLED(lfoASyncLED);

    // LFO B
    addKnob(lfoBSpeedKnob); addKnob(lfoBDepthKnob);
    addBtn(lfoBSyncBtn); addLED(lfoBSyncLED);

    // LFO Link
    addBtn(lfoLinkBtn); addLED(lfoLinkLED);

    // Morph
    addBtn(morphEnableBtn); addLED(morphEnableLED);
    addKnob(morphKnob);
    addKnob(morphLfoSpeedKnob); addKnob(morphLfoDepthKnob);

    // Master
    addKnob(inputKnob); addKnob(outputKnob); addKnob(mixKnob);
    addBtn(zOutBtn); addLED(zOutLED);
    addBtn(bypassBtn); addLED(bypassLED);

    // === Button callbacks ===

    // Toggle helper
    auto makeToggle = [this](const juce::String& paramId) {
        return [this, paramId]() {
            auto* param = processorRef.apvts.getParameter(paramId);
            param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
        };
    };

    filterAEnableBtn.onClick = makeToggle("filterAEnabled");
    filterBEnableBtn.onClick = makeToggle("filterBEnabled");
    routingBtn.onClick       = makeToggle("routingMode");
    lfoASyncBtn.onClick      = makeToggle("lfoASync");
    lfoBSyncBtn.onClick      = makeToggle("lfoBSync");
    lfoLinkBtn.onClick       = makeToggle("lfoLink");
    morphEnableBtn.onClick   = makeToggle("morphEnabled");
    zOutBtn.onClick          = makeToggle("zOutputStage");
    bypassBtn.onClick        = makeToggle("bypass");

    // Filter A type quick-set buttons
    auto setFilterA = [this](int index) {
        processorRef.apvts.getParameter("filterTypeA")->setValueNotifyingHost((float)index / 4.0f);
    };
    lpABtn.onClick = [setFilterA]() { setFilterA(0); };
    hpABtn.onClick = [setFilterA]() { setFilterA(1); };
    bpABtn.onClick = [setFilterA]() { setFilterA(2); };
    ntABtn.onClick = [setFilterA]() { setFilterA(3); };
    rgABtn.onClick = [setFilterA]() { setFilterA(4); };

    // Filter B type quick-set buttons
    auto setFilterB = [this](int index) {
        processorRef.apvts.getParameter("filterTypeB")->setValueNotifyingHost((float)index / 4.0f);
    };
    lpBBtn.onClick = [setFilterB]() { setFilterB(0); };
    hpBBtn.onClick = [setFilterB]() { setFilterB(1); };
    bpBBtn.onClick = [setFilterB]() { setFilterB(2); };
    ntBBtn.onClick = [setFilterB]() { setFilterB(3); };
    rgBBtn.onClick = [setFilterB]() { setFilterB(4); };

    // === Parameter attachments ===
    frequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "frequency", frequencyKnob);
    polesAAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "polesA", polesAKnob);
    polesBAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "polesB", polesBKnob);
    lfoASpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "lfoASpeed", lfoASpeedKnob);
    lfoADepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "lfoADepth", lfoADepthKnob);
    lfoBSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "lfoBSpeed", lfoBSpeedKnob);
    lfoBDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "lfoBDepth", lfoBDepthKnob);
    morphAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "morph", morphKnob);
    morphLfoSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "morphLfoSpeed", morphLfoSpeedKnob);
    morphLfoDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "morphLfoDepth", morphLfoDepthKnob);
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "input", inputKnob);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "output", outputKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "mix", mixKnob);

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
    bool filterAOn = *processorRef.apvts.getRawParameterValue("filterAEnabled") > 0.5f;
    bool filterBOn = *processorRef.apvts.getRawParameterValue("filterBEnabled") > 0.5f;
    bool morphOn = *processorRef.apvts.getRawParameterValue("morphEnabled") > 0.5f;
    float morph = *processorRef.apvts.getRawParameterValue("morph");
    bool parallel = *processorRef.apvts.getRawParameterValue("routingMode") > 0.5f;
    bool lfoLnk = *processorRef.apvts.getRawParameterValue("lfoLink") > 0.5f;
    float freq = *processorRef.apvts.getRawParameterValue("frequency");
    float polesA = *processorRef.apvts.getRawParameterValue("polesA");
    float polesB = *processorRef.apvts.getRawParameterValue("polesB");
    float output = *processorRef.apvts.getRawParameterValue("output");
    float mix = *processorRef.apvts.getRawParameterValue("mix");
    float input = *processorRef.apvts.getRawParameterValue("input");
    bool bypassed = *processorRef.apvts.getRawParameterValue("bypass") > 0.5f;
    bool zOut = *processorRef.apvts.getRawParameterValue("zOutputStage") > 0.5f;
    bool lfoASync = *processorRef.apvts.getRawParameterValue("lfoASync") > 0.5f;
    bool lfoBSync = *processorRef.apvts.getRawParameterValue("lfoBSync") > 0.5f;

    const char* typeShort[] = { "LP", "HP", "BP", "NT", "RG" };
    const char* typeNames[] = { "LOWPASS", "HIGHPASS", "BANDPASS", "NOTCH", "REGION" };

    juce::String tA = typeShort[juce::jlimit(0, 4, filterTypeA)];
    juce::String tB = typeShort[juce::jlimit(0, 4, filterTypeB)];

    juce::String row0, row1, row2, row3;

    // Row 0: Mode info
    if (morphOn) {
        row0 = "Z-FILTER v2 MORPH " + tA + ">" + tB;
    } else if (filterAOn && filterBOn) {
        row0 = "Z-FILTER v2 " + tA + (parallel ? "||" : "->") + tB;
    } else if (filterAOn) {
        row0 = "Z-FILTER v2 A:" + juce::String(typeNames[juce::jlimit(0, 4, filterTypeA)]);
    } else if (filterBOn) {
        row0 = "Z-FILTER v2 B:" + juce::String(typeNames[juce::jlimit(0, 4, filterTypeB)]);
    } else {
        row0 = "Z-FILTER v2 [NO FILTER]";
    }
    if (zOut) row0 += " ZOUT";

    // Row 1: Frequency
    row1 = "FREQ: " + juce::String(freq * 100.0f, 1) + "%";

    // Row 2: Poles + morph info
    if (morphOn) {
        row2 = "MORPH:" + juce::String(morph * 100.0f, 0) + "% PA:" +
               juce::String(polesA * 100.0f, 0) + " PB:" + juce::String(polesB * 100.0f, 0);
    } else {
        row2 = "PA:" + juce::String(polesA * 100.0f, 0) + "% PB:" +
               juce::String(polesB * 100.0f, 0) + "% IN:" + juce::String(input * 100.0f, 0) + "%";
    }

    // Row 3: Level/mix/bypass
    row3 = "LVL:" + juce::String(output * 100.0f, 0) + "% MIX:" +
           juce::String(mix * 100.0f, 0) + "%" + (bypassed ? " BYP" : "") +
           (lfoLnk ? " LNK" : "");

    lcd.setText(0, row0);
    lcd.setText(1, row1);
    lcd.setText(2, row2);
    lcd.setText(3, row3);

    // LEDs - Filter A type
    lpALED.setActive(filterTypeA == 0);
    hpALED.setActive(filterTypeA == 1);
    bpALED.setActive(filterTypeA == 2);
    ntALED.setActive(filterTypeA == 3);
    rgALED.setActive(filterTypeA == 4);
    filterAEnableLED.setActive(filterAOn);

    // LEDs - Filter B type
    lpBLED.setActive(filterTypeB == 0);
    hpBLED.setActive(filterTypeB == 1);
    bpBLED.setActive(filterTypeB == 2);
    ntBLED.setActive(filterTypeB == 3);
    rgBLED.setActive(filterTypeB == 4);
    filterBEnableLED.setActive(filterBOn);

    // LEDs - routing, LFO, morph, master
    routingLED.setActive(parallel);
    lfoASyncLED.setActive(lfoASync);
    lfoBSyncLED.setActive(lfoBSync);
    lfoLinkLED.setActive(lfoLnk);
    morphEnableLED.setActive(morphOn);
    zOutLED.setActive(zOut);
    bypassLED.setActive(bypassed);

    // LFO Link visual feedback: dim LFO B controls when linked
    lfoBSpeedKnob.setEnabled(!lfoLnk);
    lfoBDepthKnob.setEnabled(!lfoLnk);
    lfoBSyncBtn.setEnabled(!lfoLnk);
    lfoBSpeedKnob.setAlpha(lfoLnk ? 0.4f : 1.0f);
    lfoBDepthKnob.setAlpha(lfoLnk ? 0.4f : 1.0f);
    lfoBSyncBtn.setAlpha(lfoLnk ? 0.4f : 1.0f);

    repaint();
}

//==============================================================================
void ZFilterEditor::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
        g.drawImageAt(backgroundImage, 0, 0);
    else
        g.fillAll(juce::Colour(0xff1a1a1a));

    // Section labels
    g.setColour(juce::Colour(0xffe0e0e0));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("[FILTERS]", 10, 232, 510, 18, juce::Justification::centred);
    g.drawText("[LFO]",    490, 232, 190, 18, juce::Justification::centred);
    g.drawText("[MORPH]",  730, 232, 150, 18, juce::Justification::centred);
    g.drawText("[MASTER]", 890, 232, 300, 18, juce::Justification::centred);

    // Control labels
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colour(0xffb0b0b0));

    const int rowALblY = 300;
    const int rowBLblY = 370;

    // Filter A row labels (+40px shift right)
    g.drawText("On",    55, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("LP",   100, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("HP",   150, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("BP",   200, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("NT",   250, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("RG",   300, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("Poles", 353, rowALblY, 44, 12, juce::Justification::centred);

    // Routing label (moved to Filter A row, dynamic SER/PAR)
    bool parallel = *processorRef.apvts.getRawParameterValue("routingMode") > 0.5f;
    g.drawText(parallel ? "PAR" : "SER", 405, rowALblY, 36, 12, juce::Justification::centred);

    // Filter B row labels (+40px shift right)
    g.drawText("On",    55, rowBLblY, 36, 12, juce::Justification::centred);
    g.drawText("LP",   100, rowBLblY, 36, 12, juce::Justification::centred);
    g.drawText("HP",   150, rowBLblY, 36, 12, juce::Justification::centred);
    g.drawText("BP",   200, rowBLblY, 36, 12, juce::Justification::centred);
    g.drawText("NT",   250, rowBLblY, 36, 12, juce::Justification::centred);
    g.drawText("RG",   300, rowBLblY, 36, 12, juce::Justification::centred);
    g.drawText("Poles", 353, rowBLblY, 44, 12, juce::Justification::centred);

    // Row labels (A / B) shifted right
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffe0e0e0));
    g.drawText("A", 38, 270, 14, 20, juce::Justification::centred);
    g.drawText("B", 38, 340, 14, 20, juce::Justification::centred);

    // LFO labels (shifted left)
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colour(0xffb0b0b0));
    g.drawText("Spd",  500, rowALblY, 34, 12, juce::Justification::centred);
    g.drawText("Dpt",  545, rowALblY, 34, 12, juce::Justification::centred);
    g.drawText("Sync", 590, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("Link", 640, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("Spd",  500, rowBLblY, 34, 12, juce::Justification::centred);
    g.drawText("Dpt",  545, rowBLblY, 34, 12, juce::Justification::centred);
    g.drawText("Sync", 590, rowBLblY, 36, 12, juce::Justification::centred);

    // Morph labels
    g.drawText("On",    745, rowALblY, 30, 12, juce::Justification::centred);
    g.drawText("Mrph",  805, rowALblY, 40, 12, juce::Justification::centred);
    g.drawText("MSpd",  745, rowBLblY, 40, 12, juce::Justification::centred);
    g.drawText("MDpt",  805, rowBLblY, 40, 12, juce::Justification::centred);

    // Master labels
    // Button labels (row A)
    g.drawText("ZOut",   935, rowALblY, 36, 12, juce::Justification::centred);
    g.drawText("Byp",   1000, rowALblY, 36, 12, juce::Justification::centred);
    // Knob labels (row B)
    g.drawText("Input",  912, rowBLblY, 34, 12, juce::Justification::centred);
    g.drawText("Level",  962, rowBLblY, 34, 12, juce::Justification::centred);
    g.drawText("Mix",   1012, rowBLblY, 34, 12, juce::Justification::centred);
}

void ZFilterEditor::resized()
{
    // LCD display
    lcd.setBounds(250, 62, 560, 110);

    // Frequency knob (large)
    frequencyKnob.setBounds(990, 18, 188, 188);

    // Layout constants
    const int rowAY = 264;    // Row A: buttons/knobs Y
    const int rowBY = 334;    // Row B: buttons/knobs Y
    const int rowALedY = 252; // Row A: LED Y
    const int rowBLedY = 322; // Row B: LED Y
    const int bW = 36, bH = 22;   // Button size
    const int kS = 34;             // Knob size

    // === [FILTERS] section (x: 10-510) ===
    // Filter A row (+40px shift right)
    filterAEnableBtn.setBounds(55, rowAY, bW, bH);
    filterAEnableLED.setBounds(68, rowALedY, 10, 10);
    lpABtn.setBounds(100, rowAY, bW, bH);  lpALED.setBounds(113, rowALedY, 10, 10);
    hpABtn.setBounds(150, rowAY, bW, bH);  hpALED.setBounds(163, rowALedY, 10, 10);
    bpABtn.setBounds(200, rowAY, bW, bH);  bpALED.setBounds(213, rowALedY, 10, 10);
    ntABtn.setBounds(250, rowAY, bW, bH);  ntALED.setBounds(263, rowALedY, 10, 10);
    rgABtn.setBounds(300, rowAY, bW, bH);  rgALED.setBounds(313, rowALedY, 10, 10);
    polesAKnob.setBounds(358, rowAY, kS, kS);
    // Routing button at end of Filter A row
    routingBtn.setBounds(405, rowAY, bW, bH);
    routingLED.setBounds(418, rowALedY, 10, 10);

    // Filter B row (+40px shift right)
    filterBEnableBtn.setBounds(55, rowBY, bW, bH);
    filterBEnableLED.setBounds(68, rowBLedY, 10, 10);
    lpBBtn.setBounds(100, rowBY, bW, bH);  lpBLED.setBounds(113, rowBLedY, 10, 10);
    hpBBtn.setBounds(150, rowBY, bW, bH);  hpBLED.setBounds(163, rowBLedY, 10, 10);
    bpBBtn.setBounds(200, rowBY, bW, bH);  bpBLED.setBounds(213, rowBLedY, 10, 10);
    ntBBtn.setBounds(250, rowBY, bW, bH);  ntBLED.setBounds(263, rowBLedY, 10, 10);
    rgBBtn.setBounds(300, rowBY, bW, bH);  rgBLED.setBounds(313, rowBLedY, 10, 10);
    polesBKnob.setBounds(358, rowBY, kS, kS);

    // === [LFO] section (shifted left ~30px) ===
    // LFO A row
    lfoASpeedKnob.setBounds(500, rowAY, kS, kS);
    lfoADepthKnob.setBounds(545, rowAY, kS, kS);
    lfoASyncBtn.setBounds(590, rowAY, bW, bH);
    lfoASyncLED.setBounds(603, rowALedY, 10, 10);
    // LFO Link button at end of LFO A row
    lfoLinkBtn.setBounds(640, rowAY, bW, bH);
    lfoLinkLED.setBounds(653, rowALedY, 10, 10);

    // LFO B row
    lfoBSpeedKnob.setBounds(500, rowBY, kS, kS);
    lfoBDepthKnob.setBounds(545, rowBY, kS, kS);
    lfoBSyncBtn.setBounds(590, rowBY, bW, bH);
    lfoBSyncLED.setBounds(603, rowBLedY, 10, 10);

    // === [MORPH] section (x: 730-880) ===
    // Row 1: Enable + Morph knob
    morphEnableBtn.setBounds(745, rowAY, bW, bH);
    morphEnableLED.setBounds(758, rowALedY, 10, 10);
    morphKnob.setBounds(805, rowAY, kS, kS);

    // Row 2: Morph LFO speed + depth
    morphLfoSpeedKnob.setBounds(745, rowBY, kS, kS);
    morphLfoDepthKnob.setBounds(805, rowBY, kS, kS);

    // === [MASTER] section (x: 890-1190) ===
    // Row A (upper): ZOut and Bypass buttons
    zOutBtn.setBounds(935, rowAY, bW, bH);
    zOutLED.setBounds(948, rowALedY, 10, 10);
    bypassBtn.setBounds(1000, rowAY, bW, bH);
    bypassLED.setBounds(1013, rowALedY, 10, 10);

    // Row B (lower): Input, Level, Mix knobs
    inputKnob.setBounds(912, rowBY, kS, kS);
    outputKnob.setBounds(962, rowBY, kS, kS);
    mixKnob.setBounds(1012, rowBY, kS, kS);
}
