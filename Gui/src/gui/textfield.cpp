#include "textfield.h"

namespace gui {

	TextField::TextField(const mw::FontPtr& font) : editable_(true), text_("", font), marker_(text_), color_(0, 0, 0), alignment_(LEFT), markerDeltaTime_(0.f) {
		setPreferredSize(150, 20);
		setBackgroundColor(mw::Color(0.8, 0.8, 0.8));
	}

	TextField::TextField(std::string initialText, const mw::FontPtr& font) : editable_(true), text_(initialText, font), marker_(text_), color_(0, 0, 0), alignment_(LEFT), markerDeltaTime_(0.f) {
		setPreferredSize(150, 20);
		setBackgroundColor(mw::Color(0.8, 0.8, 0.8));
	}

	// Get the current text.
	std::string TextField::getText() const {
		return text_.getText();
	}

	// Set the current text.
	void TextField::setText(std::string text) {
		return text_.setText(text);
	}

	// Set the textfield to be editable or not.
	void TextField::setEditable(bool editable) {
		editable_ = editable;
	}

	bool TextField::isEditable() const {
		return editable_;
	}

	void TextField::setTextColor(const mw::Color& color) {
		color_ = color;
	}

	void TextField::draw(float deltaTime) {
		Component::draw(deltaTime);

		Dimension dim = getSize();
		float x = 0.0;
		switch (alignment_) {
			case Alignment::LEFT:
				x = 0;
				break;
			case Alignment::CENTER:
				x = dim.width_ * 0.5f - (float) text_.getWidth() * 0.5f;
				break;
			case Alignment::RIGHT:
				x = dim.width_ - (float) text_.getWidth();
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
					// A Utf8 string as input.
					inputFormatter_.update(keyEvent.text.text);
					break;
				case SDL_KEYDOWN:
					// Reset marker animation.
					markerDeltaTime_ = 0;
					switch (keyEvent.key.keysym.sym) {
						case SDLK_v:// Paste from clipboard!
							if ((keyEvent.key.keysym.mod & KMOD_CTRL) && SDL_HasClipboardText()) {
								char* text = SDL_GetClipboardText();
								inputFormatter_.update(SDL_GetClipboardText());
								SDL_free(text);
							}
							break;
						case SDLK_c: // Copy from textfield!
							if (keyEvent.key.keysym.mod & KMOD_CTRL) {
								SDL_SetClipboardText(inputFormatter_.getText().c_str());
							}
							break;
						case SDLK_HOME:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_HOME);
							break;
						case SDLK_END:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_END);
							break;
						case SDLK_LEFT:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_LEFT);
							break;
						case SDLK_RIGHT:
							inputFormatter_.update(InputFormatter::INPUT_MOVE_MARKER_RIGHT);
							break;
						case SDLK_BACKSPACE:
							inputFormatter_.update(InputFormatter::INPUT_ERASE_LEFT);
							break;
						case SDLK_DELETE:
							inputFormatter_.update(InputFormatter::INPUT_ERASE_RIGHT);
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
		}
	}

	void TextField::drawText(float deltaTime) {
		Dimension dim = getSize();

		color_.glColor4d();
		text_.setText(inputFormatter_.getText());
		text_.draw();

		if (editable_) {
			if (hasFocus()) {
				int index = inputFormatter_.getMarkerPosition();
				marker_.setText(text_.getText().substr(0, index));
				double x = marker_.getWidth();
				glBegin(GL_LINES);
				markerDeltaTime_ += deltaTime;
				if (markerDeltaTime_ < 0.5f) {
					glVertex2d(x + 2, text_.getCharacterSize());
					glVertex2d(x + 2, 1);
				} else if (markerDeltaTime_ > 1.f) {
					markerDeltaTime_ = 0;
				}

				glEnd();
			} else {
				markerDeltaTime_ = 0;
			}
		}
		// Draw border.
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(dim.width_, 0);

		glVertex2f(dim.width_, 0);
		glVertex2f(dim.width_, dim.height_);

		glVertex2f(dim.width_, dim.height_);
		glVertex2f(0, dim.height_);

		glVertex2f(0, dim.height_);
		glVertex2f(0, 0);
		glEnd();
	}

} // Namespace gui.
