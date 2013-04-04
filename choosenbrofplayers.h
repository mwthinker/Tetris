#ifndef CHOOSENBROFPLAYERS_H
#define CHOOSENBROFPLAYERS_H

#include <mw/texture.h>
#include <mw/sprite.h>

class ChooseNbrOfPlayers : public gui::GuiItem<MenuEvent> {
public:	
	ChooseNbrOfPlayers(int pixelSize, int upleftX, int upleftY) {
		man_ = mw::Sprite("images/man.png");
		noMan_ = mw::Sprite("images/noman.png");
		upleftX_ = upleftX;
		upleftY_ = upleftY;
		//sprite_.setDrawPixelSize(true);
		pixelSize_ = pixelSize;
		nbrOfPlayers_ = 1;
		mouseOver_ = false;
		maxPlayers_ = 4;
	}

	int getNbrOfPlayers() const {
		return nbrOfPlayers_;
	}

	void setNbrOfPlayers(int nbrOfPlayers) {
		nbrOfPlayers_ = nbrOfPlayers;
		if (nbrOfPlayers_ > maxPlayers_) {
			nbrOfPlayers_ =  0;
		}		
	}
	
	void graphicUpdate(Uint32 deltaTime) {
		glPushMatrix();		

		if (nbrOfPlayers_ == 0) {
			drawPlayer(noMan_);
		} else {
			for (int i = 0; i < nbrOfPlayers_; ++i) {
				drawPlayer(man_);
				glTranslated(pixelSize_,0,0);
			}
		}
		glPopMatrix();
	}
	
	void eventUpdate(const SDL_Event& windowEvent) {
		int x, y;
		// Gets the mouse position in the graphic coordinate system.
		GuiItem<MenuEvent>::mousePosition(x,y);
		
		bool inside = isInside(x,y);

		switch (windowEvent.type) {
		case SDL_MOUSEMOTION:
			if (inside) {
				mouseOver_ = true;
			} else {
				mouseOver_ = false;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				if (inside) {
					push(MENUEVENT_CHANGENBROFPLAYERS);
				} else {
					mouseOver_ = false;
				}
				break;
			case SDL_BUTTON_RIGHT:
				break;
			}
			break;
		};
	}
private:
	bool isInside(int x, int y) {
		int width, height;
		windowSize(width,height);
		y = height - y - upleftY_;
		//y = (y - upleftY_);
		x = x - upleftX_;
		
		int n = nbrOfPlayers_;
		if (nbrOfPlayers_ == 0) {
			n = 1;
		}
		//std::cout << "\n" << x << " " << y;

		return x > 0 &&  y > 0 && x < pixelSize_*n &&  y < pixelSize_;
	}

	void drawPlayer(mw::Sprite& sprite) {		
		int width, height;
		windowSize(width,height);

		glPushMatrix();
		glTranslated(upleftX_+pixelSize_/2.0,height - upleftY_ - pixelSize_/2.0,0);
		if (mouseOver_) {
			glScaled(1.2,1.2,1);
		}
		glScaled(pixelSize_,pixelSize_,1);
		glColor3d(1,1,1);
		
		sprite.draw();

		glPopMatrix();
	}

	bool mouseOver_;
	int nbrOfPlayers_;
	int maxPlayers_;

	mw::Sprite man_, noMan_;
	int upleftX_;
	int upleftY_;
	int pixelSize_;
};

#endif // CHOOSENBROFPLAYERS_H