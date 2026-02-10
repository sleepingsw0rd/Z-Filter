#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"

//==============================================================================
// Custom knob component - metallic style
//==============================================================================
class KnobComponent : public juce::Slider
{
public:
    enum Size { Large, Small };

    KnobComponent(Size size) : knobSize(size)
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto cx = bounds.getCentreX();
        auto cy = bounds.getCentreY();

        float sliderPos = (float)valueToProportionOfLength(getValue());
        float startAngle = juce::MathConstants<float>::pi * 1.2f;
        float endAngle = juce::MathConstants<float>::pi * 2.8f;
        float angle = startAngle + sliderPos * (endAngle - startAngle);

        if (knobSize == Large)
        {
            // Dark metallic body with gradient
            juce::ColourGradient grad(juce::Colour(0xff555555), cx, cy - radius,
                                       juce::Colour(0xff1a1a1a), cx, cy + radius, false);
            g.setGradientFill(grad);
            g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            // Subtle ring
            g.setColour(juce::Colour(0xff333333));
            g.drawEllipse(cx - radius + 1, cy - radius + 1, (radius - 1) * 2.0f, (radius - 1) * 2.0f, 1.5f);

            // White pointer line
            float pointerLen = radius * 0.75f;
            float px = cx + std::sin(angle) * pointerLen;
            float py = cy - std::cos(angle) * pointerLen;
            float ix = cx + std::sin(angle) * (radius * 0.25f);
            float iy = cy - std::cos(angle) * (radius * 0.25f);
            g.setColour(juce::Colours::white);
            g.drawLine(ix, iy, px, py, 3.0f);
        }
        else
        {
            // Small knob: flat dark fill
            g.setColour(juce::Colour(0xff2a2a2a));
            g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

            g.setColour(juce::Colour(0xff444444));
            g.drawEllipse(cx - radius + 0.5f, cy - radius + 0.5f, (radius - 0.5f) * 2.0f, (radius - 0.5f) * 2.0f, 1.0f);

            // Thin white pointer
            float pointerLen = radius * 0.7f;
            float px = cx + std::sin(angle) * pointerLen;
            float py = cy - std::cos(angle) * pointerLen;
            float ix = cx + std::sin(angle) * (radius * 0.2f);
            float iy = cy - std::cos(angle) * (radius * 0.2f);
            g.setColour(juce::Colours::white);
            g.drawLine(ix, iy, px, py, 1.5f);
        }
    }

private:
    Size knobSize;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};

//==============================================================================
// LED indicator
//==============================================================================
class LEDComponent : public juce::Component
{
public:
    LEDComponent() = default;

    void setActive(bool active)
    {
        if (isActive != active)
        {
            isActive = active;
            repaint();
        }
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        auto cx = bounds.getCentreX();
        auto cy = bounds.getCentreY();
        auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

        if (isActive)
        {
            // Glow
            g.setColour(juce::Colour(0x40ff2020));
            g.fillEllipse(cx - r * 1.5f, cy - r * 1.5f, r * 3.0f, r * 3.0f);
            // Bright red
            g.setColour(juce::Colour(0xffff3030));
            g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
            // Highlight
            g.setColour(juce::Colour(0x60ffffff));
            g.fillEllipse(cx - r * 0.4f, cy - r * 0.5f, r * 0.6f, r * 0.5f);
        }
        else
        {
            // Dim dark red
            g.setColour(juce::Colour(0xff401010));
            g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
        }
    }

private:
    bool isActive = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LEDComponent)
};

//==============================================================================
// Round button with 3D cap rendering
//==============================================================================
class RoundButtonComponent : public juce::Component
{
public:
    RoundButtonComponent() = default;

    std::function<void()> onClick;

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        float cornerSize = bounds.getHeight() * 0.42f;

        // Drop shadow
        g.setColour(juce::Colour(0x50000000));
        g.fillRoundedRectangle(bounds.translated(0.0f, 1.5f), cornerSize);

        if (pressed)
        {
            // Pressed: flat dark, shifted down
            g.setColour(juce::Colour(0xff282828));
            g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), cornerSize);
        }
        else
        {
            // Normal: gradient cap (lighter top, darker bottom)
            juce::ColourGradient grad(juce::Colour(0xff555555), bounds.getCentreX(), bounds.getY(),
                                       juce::Colour(0xff2a2a2a), bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill(grad);
            g.fillRoundedRectangle(bounds, cornerSize);

            // Top edge highlight
            g.setColour(juce::Colour(0x18ffffff));
            g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, 1.0f);
        }
    }

    void mouseDown(const juce::MouseEvent&) override
    {
        pressed = true;
        repaint();
    }

    void mouseUp(const juce::MouseEvent& e) override
    {
        pressed = false;
        repaint();
        if (getLocalBounds().contains(e.getPosition()) && onClick)
            onClick();
    }

private:
    bool pressed = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoundButtonComponent)
};

//==============================================================================
// Dot-matrix LCD display (HD44780 style) - Mini version: 4 rows x 8 cols
//==============================================================================
class DotMatrixLCD : public juce::Component
{
public:
    DotMatrixLCD();

    void setText(int row, const juce::String& text);
    void paint(juce::Graphics& g) override;

private:
    static constexpr int kRows = 4;
    static constexpr int kCols = 16;
    static constexpr int kCharW = 5;
    static constexpr int kCharH = 7;

    juce::String rows[kRows];
    bool dirty = true;

    static const uint8_t font5x7[];
    void drawChar(juce::Graphics& g, char ch, float x, float y, float dotSize, float dotPitch) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DotMatrixLCD)
};

//==============================================================================
// Main editor
//==============================================================================
class ZFilterMiniEditor : public juce::AudioProcessorEditor,
                          private juce::Timer
{
public:
    explicit ZFilterMiniEditor(ZFilterMiniProcessor&);
    ~ZFilterMiniEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    ZFilterMiniProcessor& processorRef;

    juce::Image backgroundImage;

    // LCD
    DotMatrixLCD lcd;

    // Knobs
    KnobComponent frequencyKnob { KnobComponent::Large };
    KnobComponent polesKnob     { KnobComponent::Small };
    KnobComponent outputKnob    { KnobComponent::Small };
    KnobComponent inputKnob      { KnobComponent::Small };
    KnobComponent mixKnob       { KnobComponent::Small };

    // Filter type buttons
    RoundButtonComponent lpBtn;
    RoundButtonComponent hpBtn;
    RoundButtonComponent bpBtn;
    RoundButtonComponent ntBtn;

    // Freq smooth
    RoundButtonComponent freqSmoothBtn;

    // LEDs
    LEDComponent lpLED;
    LEDComponent hpLED;
    LEDComponent bpLED;
    LEDComponent ntLED;
    LEDComponent freqSmoothLED;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> polesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    void updateDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZFilterMiniEditor)
};
