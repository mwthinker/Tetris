#ifndef GUI_TBUTTON_H
#define GUI_TBUTTON_H

#include "button.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <memory>

namespace gui {

	class TButton : public Button {
	public:
		TButton();
		TButton(std::string text, mw::FontPtr font);
		TButton(std::string text, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
			mw::Color notHover, mw::Color insideDownHover);

		void draw(float deltaTime) override;

	private:
		mw::Color focused_, onHover_, notHover_, insideDownHover_;
	};

} // Namespace gui.

#endif // GUI_TBUTTON_H
