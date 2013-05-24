#ifndef MANBUTTON_H
#define MANBUTTON_H

#include "button.h"

#include <mw/sprite.h>
#include <memory>

class ManButton : public gui::Button {
public:	
	ManButton(int pixelSize);

	unsigned int getNbr() const;

	void setNbr(unsigned int nbr);

	void draw() override;

private:
	void drawPlayer(mw::Sprite& sprite);

	bool mouseOver_;
	unsigned int nbr_;

	mw::Sprite man_, noMan_;		
	int pixelSize_;
};

typedef std::shared_ptr<ManButton> ManButtonPtr;

ManButtonPtr createManButton(int pixelSize);

#endif // MANBUTTON_H
