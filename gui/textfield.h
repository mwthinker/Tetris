#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include "guiitem.h"
#include "glcoordinate.h"
#include "inputformatter.h"

#include <mw/font.h>
#include <mw/text.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <sstream>

namespace gui {

template <typename GuiEvent>
class Text : public  GuiItem<GuiEvent> {
public:
	Text(int upleftX, int upleftY, std::string text, int characterSize, mw::FontPtr font) {
		text_ = mw::Text(text,font);
		text_.setCharacterSize(characterSize);
		upleftX_ = upleftX;
		upleftY_ = upleftY;
	}

	void graphicUpdate(Uint32 deltaTime) {
		int width, height;
		GuiItem<GuiEvent>::windowSize(width,height);
		glColor3d(1,1,1);

		glPushMatrix();
		int size = text_.getCharacterSize();
		glTranslated(upleftX_+size,height - upleftY_ - size,0);
		text_.draw();

		glPopMatrix();
	}
private:
	int upleftX_;
	int upleftY_;
	mw::Text text_;
};

template <typename GuiEvent>
class TextField : public  GuiItem<GuiEvent> {
public:
	TextField(double xPos, double yPos, int length, int characterSize, mw::FontPtr font, GuiEvent onEnterEvent) {
		position_ = GlCoordinate(xPos,yPos);
		//GuiEvent getFocusEvent
		text_ = mw::Text(">",font);
		text_.setCharacterSize(characterSize);
		onEnterEvent_ = onEnterEvent;

		width_ = (int) (length * 1.2);
		height_ = (int) (characterSize * 1.2);
		edgeThickness_ = 3;
		hasFocus_ = false,
		inputFormatter_ = InputFormatterPtr(new InputFormatter());
		time_ = 0;
	}

	// setInputFOrmatter
	void setInputFormatter(InputFormatterPtr inputFormatter) {
		inputFormatter_ = inputFormatter;
	}

	void setEventTypeOnEnter(GuiEvent onEnterEvent) {
		onEnterEvent_ = onEnterEvent;
	}

	GuiEvent getEventTypeOnEnter() const {
		return onEnterEvent_;
	}

	void setFocus(bool focus) {
		hasFocus_ = focus;
	}

	bool getFocus() const {
		return hasFocus_;
	}

	std::string getText() const {
		return text_.getText();
	}

	void graphicUpdate(Uint32 deltaTime) {
		glPushMatrix();
		int width, height;
		GuiItem<GuiEvent>::windowSize(width,height);
		glTranslated(position_.x,height-position_.y,0);
		//position_.glTranslate();
		//text_->setText(strStream_.str());
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0.9,0.9,0.9,1.0);
		glBegin(GL_QUADS);
		glVertex2d(0.0,0.0);
		glVertex2d(width_,0.0);
		glVertex2d(width_,height_);
		glVertex2d(0.0,height_);

		glColor4d(0.6,0.6,0.6,1.0);
		glVertex2d(edgeThickness_,edgeThickness_);
		glVertex2d(width_-edgeThickness_,edgeThickness_);
		glVertex2d(width_-edgeThickness_,height_-edgeThickness_);
		glVertex2d(edgeThickness_,height_-edgeThickness_);

		glEnd();

		glDisable(GL_BLEND);

		int markerPos = inputFormatter_->getMarkerPosition();
		std::string str("");

		glTranslated(edgeThickness_*0.5,edgeThickness_*0.5,0);
		glColor4d(0,0,0,1);
		if (inputFormatter_->hasInput()) {
			str = inputFormatter_->text();
			text_.setText(str);
			//textPositions_[markerPos] = text_->getWidth();
			text_.draw();
			//std::cout << inputTextField_.text();
		}

		time_ += deltaTime;
		if (time_ > 400 && hasFocus_) {
			glBegin(GL_LINES);
			glVertex2d(markerPos*text_.getWidth()*1.0/(str.size()+0.1),text_.getHeight());
			glVertex2d(markerPos*text_.getWidth()*1.0/(str.size()+0.1),0);
			glEnd();
			if (time_ > 600) {
				time_ = 0;
			}
		}

		glPopMatrix();
	}

	void eventUpdate(const SDL_Event& windowEvent) {
		int x, y;
		// Gets the mouse position in the graphic coordinate system.
		GuiItem<GuiEvent>::mousePosition(x,y);
		//std::cout << "\n "<< x << " " << y;

		bool inside = isInside(x,y);
		//std::cout << "\ninside " << inside;

		switch (windowEvent.type) {
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				if (inside) {
					hasFocus_ = true;
				} else {
					hasFocus_ = false;
				}
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				break;
			}
			break;
		case SDL_KEYDOWN: {
			if (hasFocus_) {
				// Transforms a unicode character to asci, therefor it will only work as
				// intended for unicodes that has a corrensponding ascii value.
				char key = static_cast<char>(windowEvent.key.keysym.unicode);
				inputFormatter_->update(key);
				//std::cout << inputTextField_.hasInput();

				switch (windowEvent.key.keysym.sym) {
				case SDLK_LEFT:
					inputFormatter_->update(InputFormatter::INPUT_MOVE_MARKER_LEFT);
					break;
				case SDLK_RIGHT:
					inputFormatter_->update(InputFormatter::INPUT_MOVE_MARKER_RIGHT);
					break;
				case SDLK_BACKSPACE:
					inputFormatter_->update(InputFormatter::INPUT_ERASE_LEFT);
					std::cout << "!";
					break;
				case SDLK_DELETE:
					inputFormatter_->update(InputFormatter::INPUT_ERASE_RIGHT);
					break;
				case SDLK_RETURN:
					// Fall through.
				case SDLK_KP_ENTER:
					hasFocus_ = false;
					GuiItem<GuiEvent>::push(onEnterEvent_);
					break;
                default:
                    break;
				}
			}
		}
			break;
        default:
            break;
		};
	}

protected:
	bool isInside(int x, int y) {
		int width, height;
		GuiItem<GuiEvent>::windowSize(width,height);
		y = (int) (height - y - position_.y + height_);
		//y = (y - upleftY_);
		x = (int) (x - position_.x );		

		//std::cout << "\n" << x << " " << y;

		return x > 0 &&  y > 0 && x < width_ &&  y < height_;

		//return x > position_.x &&  y > position_.y && x < position_.x + width_ &&  y < position_.y + height_;
	}
private:
	InputFormatterPtr inputFormatter_;

	GuiEvent onEnterEvent_;
	bool hasFocus_;

	GlCoordinate position_;
	mw::Text text_;
	int width_, height_;
	int edgeThickness_;

	Uint32 time_;
};

} // namespace gui

#endif // TEXTFIELD_H
