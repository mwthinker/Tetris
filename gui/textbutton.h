#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "button.h"
#include <mw/text.h>
#include <mw/font.h>
#include "glcoordinate.h"

#include <SDL_opengl.h>
#include <string>

namespace gui {

template <typename GuiEvent>
class TextButton : public Button<GuiEvent> {
public:
	TextButton(int xPos, int yPos, std::string text, int characterSize, mw::FontPtr font, GuiEvent onPushEvent) : Button<GuiEvent>(onPushEvent) {
		position_ = GlCoordinate(xPos,yPos);
		text_ = mw::Text(text,font,characterSize);
		reversY_ = false;
		selected_ = false;
	}

	void setReverseAxleY(bool reversY) {
		reversY_ = reversY;
	}

	bool getReverseAxleY() const {
		return reversY_;
	}

	void setText(std::string text) {
		text_.setText(text);
	}

	std::string getText(std::string text) {
		return text_.getText();
	}

	void eventUpdate(const SDL_Event& windowEvent) {
		Button<GuiEvent>::eventUpdate(windowEvent);

		bool mouse = false;
		switch (windowEvent.type) {
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mouse = true;
			break;
		};

		if (mouse) {
			if (Button<GuiEvent>::getMode() == Button<GuiEvent>::MODE_MOUSEOVER) {
				selected_ = true;
			} else {
				selected_ = false;
			}
		}
	}

	void graphicUpdate(Uint32 deltaTime) {
		glPushMatrix();		

		if (selected_) {
			glColor3d(0.8,0,0);
		} else {
			glColor3d(0.5,0,0);
		}

		if (reversY_) {
			int w, h;
			Button<GuiEvent>::windowSize(w,h);
			glTranslated(0,-2 * position_.y + h - 0.5 * text_.getHeight(),0);
		}

		position_.glTranslate();
		text_.draw();
		glPopMatrix();
	}

	bool isSelected() const {
		return selected_;
	}

	void setSelected(bool selected) {
		selected_ = selected;
	}
protected:
	bool isInside(int x, int y) {
		if (reversY_) {
			int w, h;
			Button<GuiEvent>::windowSize(w,h);
			y = (int) (h - y + 0.5 * text_.getHeight());
		}
		return x > position_.x &&  y > position_.y && x < position_.x + text_.getWidth() &&  y < position_.y + text_.getHeight();
	}

private:
	bool selected_;
	GlCoordinate position_;
	mw::Text text_;
	bool reversY_;
};

} // namespace gui

#endif // TEXTBUTTON_H
