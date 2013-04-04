#ifndef BARCOLOR_H
#define BARCOLOR_H

#include "bar.h"
#include "color.h"

#include <SDL_opengl.h>

namespace gui {

	class BarColor : public Bar {
	public:
		BarColor(Bar::Type type, int size, Color color);

		// Override View
		void draw(int width, int height);
	private:
		Color color_;	
	};

} // Namespace gui.

#endif // BARCOLOR_H