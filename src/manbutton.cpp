#include "manbutton.h"

#include <mw/sprite.h>

ManButton::ManButton(unsigned int max, const mw::Sprite man, const mw::Sprite cross) : max_(max) {
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

void ManButton::draw(Uint32 deltaTime) {
	glPushMatrix();
	gui::Dimension dim = getSize();
	if (nbr_ == 0) {
		drawPlayer(man_);
		drawPlayer(cross_);
	} else {
		for (unsigned int i = 0; i < nbr_; ++i) {
			drawPlayer(man_);
			glTranslated(dim.height_, 0, 0);
		}
	}
	glPopMatrix();
}

void ManButton::drawPlayer(mw::Sprite& sprite) {
	glPushMatrix();
	gui::Dimension dim = getSize();
	glTranslated(dim.height_ / 2.0, dim.height_ / 2.0, 0);
	if (mouseInside_) {
		glScaled(1.2,1.2,1);
	}
	glScaled(dim.height_, dim.height_, 1);
	glColor3d(1,1,1);
	sprite.draw();
	glPopMatrix();
}

void ManButton::handleMouse(const SDL_Event& mouseEvent) {
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
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void ManButton::mouseMotionLeave() {
	mouseInside_ = false;
}
