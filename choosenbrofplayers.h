#ifndef CHOOSENBROFPLAYERS_H
#define CHOOSENBROFPLAYERS_H

#include "button.h"
#include "gamesprite.h"

#include <mw/texture.h>
#include <mw/sprite.h>

#include <memory>

namespace gui {

	class ChooseNbrOfPlayers : public Button {
	public:	
		ChooseNbrOfPlayers(int pixelSize) {
			man_ = spriteMan;
			noMan_ = spriteNoMan;
			
			pixelSize_ = pixelSize;
			nbrOfPlayers_ = 1;
			mouseOver_ = false;
			setHeight(pixelSize_);
			setWidth(pixelSize_);
		}

		int getNbrOfPlayers() const {
			return nbrOfPlayers_;
		}

		void setNbrOfPlayers(int nbrOfPlayers) {
			nbrOfPlayers_ = nbrOfPlayers;
			setWidth(pixelSize_*nbrOfPlayers_);
		}

		void draw() override {
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

	private:
		void drawPlayer(mw::Sprite& sprite) {
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

		bool mouseOver_;
		int nbrOfPlayers_;

		mw::Sprite man_, noMan_;		
		int pixelSize_;
	};

	typedef std::shared_ptr<ChooseNbrOfPlayers> ChooseNbrOfPlayersPtr;

	ChooseNbrOfPlayersPtr createChoosenNbrOfPlayers(int pixelSize) {
		return ChooseNbrOfPlayersPtr(new ChooseNbrOfPlayers(pixelSize));
	}

} // Namespace gui.

#endif // CHOOSENBROFPLAYERS_H
