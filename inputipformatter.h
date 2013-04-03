#ifndef INPUTIPFORMATTER_H
#define INPUTIPFORMATTER_H

#include <SDL_opengl.h>

#include "gui/guiitem.h"
#include "gui/inputformatter.h"

class PortFormatter : public gui::InputFormatter {
public:
	bool isValidKey(char key, int index) const {
		// Only a number or a dot is allowed.
		if (!(key >= '0' && key <= '9')) {
			return false;
		}
				
		const int size = getSize();
		
		if (size > 4) {
			return false;
		}
	
		// Everything seems fine.
		return true;
	}
};

class InputIpFormatter : public gui::InputFormatter {
public:
	InputIpFormatter() : gui::InputFormatter(15) {
	}
private:
	bool isValidKey(char key, int index) const {
		// Only a number or a dot is allowed.
		if ( !((key >= '0' && key <= '9') || key == '.') ) {
			return false;
		}

		// Can't begin with a dot.
		if (key == '.' && index == 0) {
			return false;
		}

		// Two dots in a row is not allowed.
		if (key == '.' && index != 0 && characterAt(index-1) == '.') {
			return false;
		}
				
		const int size = getSize();
		
		
		if (key == '.') {
			int numberOfDots = 1;
			for (int i = 0; i < size; ++i) {
				if (characterAt(i) == '.' && i != index) {
					++numberOfDots;
				}
			}

			// Max 3 dots is allowed.
			if (numberOfDots > 3) {
				return false;
			}
		}
		
		// Max 3 number in a row is allowed.	
	
		// Everything seems fine.
		return true;
	}

	int getInteger(char number) const {
		return number - '0';
	}
};

#endif // INPUTIPFORMATTER_H