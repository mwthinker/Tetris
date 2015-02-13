#ifndef GUI_CLASSES_H
#define GUI_CLASSES_H

#include "tetrisentry.h"

#include <gui/panel.h>
#include <gui/label.h>
#include <gui/button.h>
#include <gui/textfield.h>
#include <gui/checkbox.h>
#include <gui/flowlayout.h>
#include <gui/borderlayout.h>

namespace {

    class Bar : public gui::Panel {
    public:
        Bar(TetrisEntry entry) {
            setPreferredSize(entry.getDeepChildEntry("window bar height").getFloat(), entry.getDeepChildEntry("window bar height").getFloat());
            setBackgroundColor(entry.getDeepChildEntry("window bar color").getColor());
            setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);
        }
    };

    class Background : public gui::Panel {
    public:
        Background(const mw::Sprite& background) {
            setBackground(background);
            setLayout<gui::BorderLayout>();
        }
    };

    class Label : public gui::Label {
    public:
        Label(std::string text, mw::Font font, TetrisEntry entry) : gui::Label(text, font) {
            setTextColor(entry.getDeepChildEntry("window label textColor").getColor());
            setBackgroundColor(entry.getDeepChildEntry("window label backgroundColor").getColor());
        }
    };

    class Button : public gui::Button {
    public:
        Button(std::string text, mw::Font font, TetrisEntry entry) : gui::Button(text, font) {
            setFocusColor(entry.getDeepChildEntry("window button focusColor").getColor());
            setTextColor(entry.getDeepChildEntry("window button textColor").getColor());
            setHoverColor(entry.getDeepChildEntry("window button hoverColor").getColor());
            setPushColor(entry.getDeepChildEntry("window button pushColor").getColor());
            setBackgroundColor(entry.getDeepChildEntry("window button backgroundColor").getColor());
            setBorderColor(entry.getDeepChildEntry("window button borderColor").getColor());
            setAutoSizeToFitText(true);
        }
    };

    class CheckBox : public gui::CheckBox {
    public:
        CheckBox(std::string text, const mw::Font& font, TetrisEntry entry)
            : gui::CheckBox(text,
                font,
                entry.getDeepChildEntry("window checkBox boxImage").getSprite(),
                entry.getDeepChildEntry("window checkBox checkImage").getSprite()) {

            setTextColor(entry.getDeepChildEntry("window checkBox textColor").getColor());
            setBackgroundColor(entry.getDeepChildEntry("window checkBox backgroundColor").getColor());
            setBoxColor(entry.getDeepChildEntry("window checkBox boxColor").getColor());
            setCheckColor(entry.getDeepChildEntry("window checkBox checkColor").getColor());
        }
    };

    class TransparentPanel : public gui::Panel {
    public:
        TransparentPanel(float preferredWidth = 100, float preferredHeight = 100) {
            setBackgroundColor(1, 1, 1, 0);
            setPreferredSize(preferredWidth, preferredHeight);
        }
    };

    using TextField = gui::TextField;

} // Namespace anonymous.

#endif // GUI_CLASSES_H
