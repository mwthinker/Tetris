#ifndef BARCOLOR_H
#define BARCOLOR_H

#include "bar.h"

#include <mw/color.h>

#include <SDL_opengl.h>

namespace gui {

	class BarColor : public Bar {
	public:
		BarColor(Bar::Type type, int size, mw::Color color);
				
		void draw(int width, int height) override;
	private:
		mw::Color color_;
	};

} // Namespace gui.

#endif // BARCOLOR_H