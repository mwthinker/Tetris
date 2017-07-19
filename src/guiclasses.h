#ifndef GUI_CLASSES_H
#define GUI_CLASSES_H

#include "tetrisdata.h"

#include <gui/panel.h>
#include <gui/label.h>
#include <gui/button.h>
#include <gui/textfield.h>
#include <gui/checkbox.h>
#include <gui/flowlayout.h>
#include <gui/borderlayout.h>
#include <gui/combobox.h>

namespace {

    class Bar : public gui::Panel {
    public:
        Bar() {
            setPreferredSize(TetrisData::getInstance().getWindowBarHeight(), TetrisData::getInstance().getWindowBarHeight());
            setBackgroundColor(TetrisData::getInstance().getWindowBarColor());
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
        Label(std::string text, mw::Font font) : gui::Label(text, font) {
			auto color = TetrisData::getInstance().getLabelTextColor();
			auto color2 = TetrisData::getInstance().getLabelBackgroundColor();
            setTextColor(TetrisData::getInstance().getLabelTextColor());
            setBackgroundColor(TetrisData::getInstance().getLabelBackgroundColor());
        }
    };

    class Button : public gui::Button {
    public:
        Button(std::string text, mw::Font font) : gui::Button(text, font) {
			setFocusColor(TetrisData::getInstance().getButtonFocusColor());
			setTextColor(TetrisData::getInstance().getButtonTextColor());
			setHoverColor(TetrisData::getInstance().getButtonHoverColor());
			setPushColor(TetrisData::getInstance().getButtonPushColor());
			setBackgroundColor(TetrisData::getInstance().getButtonBackgroundColor());
			setBorderColor(TetrisData::getInstance().getButtonBorderColor());
            setAutoSizeToFitText(true);
        }
    };

    class CheckBox : public gui::CheckBox {
    public:
        CheckBox(std::string text, const mw::Font& font)
            : gui::CheckBox(text, font,
			TetrisData::getInstance().getCheckboxBoxSprite(),
			TetrisData::getInstance().getCheckboxCheckSprite()) {

			setTextColor(TetrisData::getInstance().getCheckboxTextColor());
			setBackgroundColor(TetrisData::getInstance().getCheckboxBackgroundColor());
			setBoxColor(TetrisData::getInstance().getCheckboxBoxColor());
			setCheckColor(TetrisData::getInstance().getChecboxCheckColor());
        }
    };

    class TransparentPanel : public gui::Panel {
    public:
        TransparentPanel(float preferredWidth = 100, float preferredHeight = 100) {
            setBackgroundColor(1, 1, 1, 0);
            setPreferredSize(preferredWidth, preferredHeight);
        }

		virtual ~TransparentPanel() = default;

    };

    using TextField = gui::TextField;

	class ComboBox : public gui::ComboBox {
	public:
		ComboBox(mw::Font font) : gui::ComboBox(font, TetrisData::getInstance().getComboBoxShowDropDownSprite()) {
			setFocusColor(TetrisData::getInstance().getComboBoxFocusColor());
			setTextColor(TetrisData::getInstance().getComboBoxTextColor());
			setSelectedBackgroundColor(TetrisData::getInstance().getComboBoxSelectedBackgroundColor());
			setSelectedTextColor(TetrisData::getInstance().getComboBoxSelectedTextColor());
			setShowDropDownColor(TetrisData::getInstance().getComboBoxShowDropDownColor());
			setBackgroundColor(TetrisData::getInstance().getComboBoxBackgroundColor());
			setBorderColor(TetrisData::getInstance().getComboBoxBorderColor());
		}
	};

} // Namespace anonymous.

#endif // GUI_CLASSES_H
