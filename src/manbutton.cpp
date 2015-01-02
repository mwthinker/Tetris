#include "manbutton.h"

#include <mw/sprite.h>

ManButton::ManButton(unsigned int max, const mw::Sprite& man, const mw::Sprite& cross) : max_(max) {
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
	glUseProgram();
	mw::Matrix44 model = getModelMatrix();
	setGlColorU(1, 1, 1);
	
	gui::Dimension dim = getSize();
	if (nbr_ == 0) {
		drawPlayer(model, man_);
		drawPlayer(model, cross_);
	} else {
		for (unsigned int i = 0; i < nbr_; ++i) {
			drawPlayer(model, man_);
			mw::translate2D(model, dim.height_, 0);
		}
	}
}

void ManButton::drawPlayer(mw::Matrix44 model, mw::Sprite& sprite) {
	gui::Dimension dim = getSize();
	mw::translate2D(model, dim.height_ / 2.f, dim.height_ / 2.f);
	if (mouseInside_) {
		mw::scale2D(model, 1.2f, 1.2f);
	}
	setGlModelMatrixU(model * mw::getScaleMatrix44(dim.height_, dim.height_));
	
	sprite.draw();
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
			}
			break;
	}
}

void ManButton::mouseMotionLeave() {
	mouseInside_ = false;
}
