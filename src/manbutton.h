#ifndef MANBUTTON_H
#define MANBUTTON_H

#include <gui/button.h>

#include <mw/sprite.h>
#include <memory>

class ManButton : public gui::Button {
public:	
	ManButton(float pixelSize, const mw::Sprite man, const mw::Sprite cross);

	unsigned int getNbr() const;
	void setNbr(unsigned int nbr);

	void draw(float deltaTime) override;
private:
	void drawPlayer(mw::Sprite& sprite);
	unsigned int nbr_;

	mw::Sprite man_, cross_;
	float pixelSize_;
};

#endif // MANBUTTON_H
