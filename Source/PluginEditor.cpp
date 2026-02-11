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
ZFilterMiniEditor::ZFilterMiniEditor(ZFilterMiniProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(723, 342);

    // Load background image
    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::GUIminiLogo_png, BinaryData::GUIminiLogo_pngSize);

    // LCD
    addAndMakeVisible(lcd);

    // Knobs
    addAndMakeVisible(frequencyKnob);
    addAndMakeVisible(polesKnob);
    addAndMakeVisible(outputKnob);
    addAndMakeVisible(inputKnob);
    addAndMakeVisible(mixKnob);

    // Buttons
    addAndMakeVisible(lpBtn);
    addAndMakeVisible(hpBtn);
    addAndMakeVisible(bpBtn);
    addAndMakeVisible(ntBtn);
    addAndMakeVisible(freqSmoothBtn);

    // LEDs
    addAndMakeVisible(lpLED);
    addAndMakeVisible(hpLED);
    addAndMakeVisible(bpLED);
    addAndMakeVisible(ntLED);
    addAndMakeVisible(freqSmoothLED);

    // Filter type button callbacks (4 types: divisor = 3.0f)
    auto setFilter = [this](int index) {
        processorRef.apvts.getParameter("filterType")->setValueNotifyingHost((float)index / 3.0f);
    };
    lpBtn.onClick = [setFilter]() { setFilter(0); };
    hpBtn.onClick = [setFilter]() { setFilter(1); };
    bpBtn.onClick = [setFilter]() { setFilter(2); };
    ntBtn.onClick = [setFilter]() { setFilter(3); };

    freqSmoothBtn.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("freqSmooth");
        param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
    };

    // Parameter attachments
    frequencyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "frequency", frequencyKnob);
    polesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "poles", polesKnob);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "output", outputKnob);
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "input", inputKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "mix", mixKnob);

    updateDisplay();
    startTimerHz(15);
}

ZFilterMiniEditor::~ZFilterMiniEditor()
{
    stopTimer();
}

void ZFilterMiniEditor::timerCallback()
{
    updateDisplay();
}

void ZFilterMiniEditor::updateDisplay()
{
    int filterType = (int)*processorRef.apvts.getRawParameterValue("filterType");
    float freq = *processorRef.apvts.getRawParameterValue("frequency");
    float input = *processorRef.apvts.getRawParameterValue("input");
    float poles = *processorRef.apvts.getRawParameterValue("poles");
    float mix = *processorRef.apvts.getRawParameterValue("mix");
    bool freqSmooth = *processorRef.apvts.getRawParameterValue("freqSmooth") > 0.5f;

    // Row 0: "Z-FILTER" (8 chars exactly)
    lcd.setText(0, "Z-FILTER");

    // Row 1: "MINI" + filter type abbreviation + SM indicator
    const char* typeShort[] = { " LP", " HP", " BP", " NT" };
    lcd.setText(1, "MINI" + juce::String(typeShort[juce::jlimit(0, 3, filterType)]) + (freqSmooth ? " SM" : ""));

    // Row 2: F and I values (e.g. "F:50 I:50")
    lcd.setText(2, "F:" + juce::String((int)(freq * 100.0f)).paddedLeft(' ', 3) +
                    " I:" + juce::String((int)(input * 100.0f)));

    // Row 3: P and M values (e.g. "P:50 M:99")
    lcd.setText(3, "P:" + juce::String((int)(poles * 100.0f)).paddedLeft(' ', 3) +
                    " M:" + juce::String((int)(mix * 100.0f)));

    // LEDs
    lpLED.setActive(filterType == 0);
    hpLED.setActive(filterType == 1);
    bpLED.setActive(filterType == 2);
    ntLED.setActive(filterType == 3);
    freqSmoothLED.setActive(freqSmooth);

    repaint();
}

//==============================================================================
void ZFilterMiniEditor::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
        g.drawImageAt(backgroundImage, 0, 0);
    else
        g.fillAll(juce::Colour(0xff1a1a1a));

    g.setColour(juce::Colour(0xffb0b0b0));
    g.setFont(juce::Font(11.0f));

    const int labelY = 288;
    const int spacing = 60;
    const int startX = 168;      // shifted left 50px
    const int labelW = 50;

    // Frequency label (under big knob, centered on 15 + 150/2 = 90)
    g.drawText("Freq", 540, 190, 150, 14, juce::Justification::centred);

    // Button labels
    auto lx = [&](int i) { return startX + i * spacing - labelW / 2; };
    g.drawText("LP",    lx(0), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("HP",    lx(1), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("BP",    lx(2), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("NT",    lx(3), labelY, labelW, 14, juce::Justification::centred);

    // Knob labels
    g.drawText("Input", lx(4), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("Poles", lx(5), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("Level", lx(6), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("Mix",   lx(7), labelY, labelW, 14, juce::Justification::centred);
    g.drawText("Smth",  668, 260, 50, 14, juce::Justification::centredLeft);
}

void ZFilterMiniEditor::resized()
{
    // LCD display — 4 rows x 8 cols, positioned over the green area
    lcd.setBounds(250, 50, 236, 120);

    // Big frequency knob — prominent, left side, overlapping panel and strip
    frequencyKnob.setBounds(540, 35, 150, 150);

    // Bottom strip controls — evenly spaced, moved up 20px
    const int btnW = 36, btnH = 22;
    const int kS = 38;           // small knob size
    const int ledS = 10;

    const int controlY = 248;    // top of knobs
    const int btnY = 256;        // top of buttons (vertically centered with knobs)
    const int ledY = 240;        // LEDs above buttons
    const int spacing = 60;      // uniform center-to-center spacing
    const int startX = 168;      // center of first control (LP button) — shifted left 50px

    // Filter type buttons + LEDs
    auto btnCX = [&](int i) { return startX + i * spacing; };

    lpBtn.setBounds(btnCX(0) - btnW / 2, btnY, btnW, btnH);
    lpLED.setBounds(btnCX(0) - ledS / 2, ledY, ledS, ledS);

    hpBtn.setBounds(btnCX(1) - btnW / 2, btnY, btnW, btnH);
    hpLED.setBounds(btnCX(1) - ledS / 2, ledY, ledS, ledS);

    bpBtn.setBounds(btnCX(2) - btnW / 2, btnY, btnW, btnH);
    bpLED.setBounds(btnCX(2) - ledS / 2, ledY, ledS, ledS);

    ntBtn.setBounds(btnCX(3) - btnW / 2, btnY, btnW, btnH);
    ntLED.setBounds(btnCX(3) - ledS / 2, ledY, ledS, ledS);

    // Small knobs — Input, Poles, Level, Mix
    inputKnob.setBounds(btnCX(4) - kS / 2, controlY, kS, kS);
    polesKnob.setBounds(btnCX(5) - kS / 2, controlY, kS, kS);
    outputKnob.setBounds(btnCX(6) - kS / 2, controlY, kS, kS);
    mixKnob.setBounds(btnCX(7) - kS / 2, controlY, kS, kS);

    // Freq Smooth button + LED (position 8)
    freqSmoothBtn.setBounds(btnCX(8) - btnW / 2, btnY, btnW, btnH);
    freqSmoothLED.setBounds(btnCX(8) - ledS / 2, ledY, ledS, ledS);
}
