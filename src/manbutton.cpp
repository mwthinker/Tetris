#include "manbutton.h"

#include <mw/sprite.h>

ManButton::ManButton(unsigned int max, const mw::Sprite& man, const mw::Sprite& cross) 
	: max_(max), active_(true) {
	gui::Component::setBackgroundColor(1, 1, 1, 0);
	gui::Component::setBorderColor(1, 1, 1, 0);

	man_ = man;
	cross_ = cross;

	nbr_ = 1;
	mouseInside_ = false;

	setPreferredSize(30, 30);
}

unsigned int ManButton::getNbr() const {
	return nbr_;
}

void ManButton::setNbr(unsigned int nbr) {
	nbr_ = nbr;
	if (nbr == 0) {
		// Over crossed button has still the size of one button.
		nbr = 1;
	}
	// Set the correct size of the button.
	gui::Dimension dim = getPreferredSize();
	setPreferredSize(dim.height_ * nbr, dim.height_);
}

void ManButton::draw(const gui::Graphic& graphic, double deltaTime) {
	gui::Component::draw(graphic, deltaTime);
	graphic.setColor(1, 1, 1);
	gui::Dimension dim = getSize();

	float scale = 1;
	float delta = 0;
	if (mouseInside_) {
		scale = 1.2f;
		delta = 0.1f * dim.height_; // In order to make each sprite to be centered when scaled.
	}
	if (nbr_ == 0) {
		graphic.drawSprite(man_, -delta, -delta, dim.height_ * scale, dim.height_ * scale);
		graphic.drawSprite(cross_, -delta, -delta, dim.height_ * scale, dim.height_ * scale);
	} else {
		for (unsigned int i = 0; i < nbr_; ++i) {
			graphic.drawSprite(man_, dim.height_ * i - delta, -delta, dim.height_ * scale, dim.height_ * scale);
		}
	}
}

void ManButton::handleMouse(const SDL_Event& mouseEvent) {
	if (active_) {
		switch (mouseEvent.type) {
			case SDL_MOUSEMOTION:
				mouseInside_ = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseInside_ = true;
				break;
			case SDL_MOUSEBUTTONUP:
				switch (mouseEvent.button.button) {
					case SDL_BUTTON_LEFT:
						nbr_ = (nbr_ + 1) % (max_ + 1);
						setNbr(nbr_);
						doAction();
						break;
					case SDL_BUTTON_RIGHT:
						nbr_ = (nbr_ + max_) % (max_ + 1);
						setNbr(nbr_);
						doAction();
						break;
				}
				break;
		}
	}
}

void ManButton::mouseMotionLeave() {
	mouseInside_ = false;
}
