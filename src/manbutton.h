#ifndef MANBUTTON_H
#define MANBUTTON_H

#include <gui/button.h>

#include <mw/sprite.h>

class ManButton : public gui::Component {
public:
	ManButton(unsigned int max, const mw::Sprite& man, const mw::Sprite& cross);

	unsigned int getNbr() const;
	void setNbr(unsigned int nbr);

	inline unsigned int getMax() const {
		return max_;
	}

	void draw(Uint32 deltaTime) override;

protected:
	void handleMouse(const SDL_Event& mouseEvent) override;
	void mouseMotionLeave() override;

private:
	void drawPlayer(mw::Matrix44 model, mw::Sprite& sprite);
	unsigned int nbr_;

	mw::Sprite man_, cross_;
	unsigned int max_;

	bool mouseInside_;
};

#endif // MANBUTTON_H
