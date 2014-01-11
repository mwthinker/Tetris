#include "textfield.h"

namespace gui {

	TextField::TextField(const mw::FontPtr& font) {
		init("", font);
	}

	TextField::TextField(std::string initialText, const mw::FontPtr& font) {
		init(initialText, font);
	}

	void TextField::init(std::string initialText, const mw::FontPtr& font) {
		font_ = font;
		inputFormatter_.update(initialText.c_str());
		editable_ = true;
		setPreferredSize(150, 20);
		setBackgroundColor(mw::Color(0.8, 0.8, 0.8));
		textColor_ = mw::Color(0, 0, 0);
		text_ = mw::Text(inputFormatter_.getText(), font);
		// One pixel to the right of the last character.
		markerWidth_ = (float) text_.getWidth() + 1;
		alignment_ = LEFT;
		markerDeltaTime_ = 0;
		markerChanged_ = false;
	}

	// Get the current text.
	std::string TextField::getText() const {
		return text_.getText();
	}

	// Set the current text.
	void TextField::setText(std::string text) {
		text_.setText(text);
		markerWidth_ = (float) text_.getWidth() - 1;
	}

	// Set the textfield to be editable or not.
	void TextField::setEditable(bool editable) {
		editable_ = editable;
	}

	bool TextField::isEditable() const {
		return editable_;
	}

	void TextField::setTextColor(const mw::Color& color) {
		textColor_ = color;
	}

	void TextField::draw(Uint32 deltaTime) {
		Component::draw(deltaTime);

		Dimension dim = getSize();
		float x = 0.0;
		// Text should 
		switch (alignment_) {
			case Alignment::LEFT:
				x = 2;
				break;
			case Alignment::CENTER:
				x = dim.width_ * 0.5f - ((float) text_.getWidth() - 2) * 0.5f;
				break;
			case Alignment::RIGHT:
				x = dim.width_ - (float) text_.getWidth() - 2;
				break;
		}

		glPushMatrix();
		glTranslatef(x, 0, 0);
		drawText(deltaTime);
		glPopMatrix();
	}

	TextField::Alignment TextField::getAlignment() const {
		return alignment_;
	}

	void TextField::setAlignment(TextField::Alignment alignment) {
		alignment_ = alignment;
	}

	void TextField::handleKeyboard(const SDL_Event& keyEvent) {
		if (editable_) {
			switch (keyEvent.type) {
				case SDL_TEXTINPUT:
				{
					// Update only if the glyph is avaiable.
					if (0 == TTF_SizeUTF8(font_->getTtfFont(), keyEvent.text.text, 0, 0)) {
						// A Utf8 string as input.
						inputFormatter_.update(keyEvent.text.text);
						text_.setText(inputFormatter_.getText());
						markerChanged_ = true;
					}
					break;
				}
				case SDL_KEYDOWN:
					// Reset marker animation.
					markerDeltaTime_ = 0;
					switch (keyEvent.key.keysym.sym) {
						case SDLK_v:// Paste from clipboard!
							if ((keyEvent.key.keysym.mod & KMOD_CTRL) && SDL_HasClipboardText()) {
								char* text = SDL_GetClipboardText();
								inputFormatter_.update(SDL_GetClipboardText());
								text_.setText(inputFormatter_.getText());
								markerChanged_ = true;
								SDL_free(text);
							}
							break;
						case SDLK_c: // Copy from textfield!
							if (keyEvent.key.keysym.mod & KMOD_CTRL) {
								SDL_SetClipboardText(inputFormatter_.getText().c_str());
							}
							break;
						case SDLK_x: // Cut from textfield!
							if (keyEvent.key.keysym.mod & KMOD_CTRL) {
								SDL_SetClipboardText(inputFormatter_.getText().c_str());
								inputFormatter_.clear();
								text_.setText(inputFormatter_.getText());
								markerChanged_ = true;
							}
							break;
						case SDLK_HOME:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_HOME);
							markerChanged_ = true;
							break;
						case SDLK_END:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_END);
							markerChanged_ = true;
							break;
						case SDLK_LEFT:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_LEFT);
							markerChanged_ = true;
							break;
						case SDLK_RIGHT:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_RIGHT);
							markerChanged_ = true;
							break;
						case SDLK_BACKSPACE:
							inputFormatter_.update(InputFormatter::INPUT_ERASE_LEFT);
							text_.setText(inputFormatter_.getText());
							markerChanged_ = true;
							break;
						case SDLK_DELETE:
							inputFormatter_.update(InputFormatter::INPUT_ERASE_RIGHT);
							text_.setText(inputFormatter_.getText());
							markerChanged_ = true;
							break;
						case SDLK_RETURN:
							// Fall through!
						case SDLK_KP_ENTER:
							doAction();
							break;
						default:
							break;
					}
					break;
				default:
					// Uninteresting events.
					break;
			}
			if (markerChanged_) {
				markerChanged_ = false;
				int index = inputFormatter_.getMarkerPosition();
				std::string leftText = getText().substr(0, index);
				int w, h;
				TTF_SizeUTF8(font_->getTtfFont(), leftText.c_str(), &w, &h);
				// One pixel to the right of the last character.
				markerWidth_ = (float) w + 1;
			}
		}
	}

	void TextField::drawText(Uint32 deltaTime) {
		textColor_.glColor4d();
		text_.draw();

		if (editable_) {
			if (hasFocus()) {
				glBegin(GL_LINES);
				markerDeltaTime_ += deltaTime;
				if (markerDeltaTime_ < 500) {
					glVertex2d(markerWidth_, text_.getCharacterSize());
					glVertex2d(markerWidth_, 1);
				} else if (markerDeltaTime_ > 1000) {
					markerDeltaTime_ = 0;
				}

				glEnd();
			} else {
				markerDeltaTime_ = 0;
			}
		}
	}

} // Namespace gui.
