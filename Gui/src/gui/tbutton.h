#ifndef TBUTTON_H
#define TBUTTON_H

#include "button.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <memory>

namespace gui {

	class TButton : public Button {
	public:
		TButton(std::string text, mw::FontPtr font);
		TButton(std::string text, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
			mw::Color notHover, mw::Color insideDownHover);

		void draw(float deltaTime) override;

	private:
		mw::Color textColor_, focused_, onHover_, notHover_, insideDownHover_;
		mw::Text text_;
		int size_;
	};

} // Namespace gui.

#endif // TBUTTON_H
