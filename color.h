#ifndef COLOR_H
#define COLOR_H

#include <SDL_opengl.h>

// Holds RGB-color needed to set the color in openGl.
class Color {
public:
	Color() {
		red_ = 1.0;
		green_ = 1.0;
		blue_ = 1.0;
		alpha_ = 1.0;
	}

	Color(double red, double green, double blue, double alpha = 1.0) {
		red_ = red;
		green_ = green;
		blue_ = blue;
		alpha_ = alpha;
	}

	void glColor3d() {
		::glColor3d(red_,green_,blue_);
	}

	void glColor4d() {
		::glColor4d(red_,green_,blue_,alpha_);
	}

	double red_, green_, blue_, alpha_;
};

#endif // COLOR_H