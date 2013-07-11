#ifndef MANBUTTON_H
#define MANBUTTON_H

#include "button.h"

#include <mw/sprite.h>
#include <memory>

class ManButton;
typedef std::shared_ptr<ManButton> ManButtonPtr;

class ManButton : public gui::Button {
public:	
	ManButton(int pixelSize, const mw::Sprite man, const mw::Sprite cross);

	unsigned int getNbr() const;

	void setNbr(unsigned int nbr);

	void draw() override;

private:
	void drawPlayer(mw::Sprite& sprite);

	bool mouseOver_;
	unsigned int nbr_;

	mw::Sprite man_, cross_;
	int pixelSize_;
};

ManButtonPtr createManButton(int pixelSize, const mw::Sprite man, const mw::Sprite cross);

#endif // MANBUTTON_H
