#ifndef GLCOORDINATE_H
#define GLCOORDINATE_H

#include <SDL_opengl.h>

class GlCoordinate {
public:
	GlCoordinate() {
		x = 0;
		y = 0;
	}

	GlCoordinate(double xCoord, double yCoord) {
		x = xCoord;
		y = yCoord;
	}

	void glTranslate() {
		glTranslated(x,y,0);
	}

	double x, y;
};

#endif // GLCOORDINATE_H