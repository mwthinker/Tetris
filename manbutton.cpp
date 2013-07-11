#include "manbutton.h"

#include <mw/sprite.h>

ManButton::ManButton(int pixelSize, const mw::Sprite man, const mw::Sprite cross) {
	man_ = man;
	cross_ = cross;

	pixelSize_ = pixelSize;
	nbr_ = 1;
	mouseOver_ = false;
	setHeight(pixelSize_);
	setWidth(pixelSize_);
}

unsigned int ManButton::getNbr() const {
	return nbr_;
}

void ManButton::setNbr(unsigned int nbr) {
	nbr_ = nbr;
	if (nbr == 0) {
		nbr = 1;
	}
	setWidth(pixelSize_*nbr);
}

void ManButton::draw() {
	glPushMatrix();

	if (nbr_ == 0) {
		drawPlayer(man_);
		drawPlayer(cross_);
	} else {
		for (unsigned int i = 0; i < nbr_; ++i) {
			drawPlayer(man_);
			glTranslated(pixelSize_,0,0);
		}
	}
	glPopMatrix();
}

void ManButton::drawPlayer(mw::Sprite& sprite) {
	glPushMatrix();
	glTranslated(pixelSize_/2.0,pixelSize_/2.0,0);
	if (isMouseInside()) {
		glScaled(1.2,1.2,1);
	}
	glScaled(pixelSize_,pixelSize_,1);
	glColor3d(1,1,1);
	sprite.draw();
	glPopMatrix();
}

ManButtonPtr createManButton(int pixelSize, const mw::Sprite man, const mw::Sprite cross) {
	return ManButtonPtr(new ManButton(pixelSize, man, cross));
}
