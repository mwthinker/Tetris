#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include <gui/button.h>

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

class TextButton : public gui::Button {
public:
	TextButton();
	TextButton(std::string text, mw::FontPtr font);
	TextButton(std::string text, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
		mw::Color notHover, mw::Color insideDownHover);

	void draw(float deltaTime) override;

private:
	mw::Color focused_, onHover_, notHover_, insideDownHover_;
};

#endif // TEXTBUTTON_H
