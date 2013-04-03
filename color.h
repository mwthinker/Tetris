#ifndef COLOR_H
#define COLOR_H

#include <SDL_opengl.h>

class Color {
public:
    Color() {
        red = 1;
        green = 1;
        blue = 1;
		alpha = 0;
    }

    Color(double r, double g, double b) {
        red = r;
        green = g;
        blue = b;
		alpha = 0;
    }

	Color(double r, double g, double b, double a) {
        red = r;
        green = g;
        blue = b;
		alpha = a;
    }

    void glColor() const {
        glColor4d(red,green, blue, alpha);
    }

    double red, green, blue, alpha;
};

const Color RED(1,0,0);
const Color GREEN(0,1,0);
const Color BLUE(0,0,1);

#endif // COLOR_H
