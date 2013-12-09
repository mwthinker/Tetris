#ifndef COMPONENT_H
#define COMPONENT_H

#include <mw/signal.h>

namespace gui {

	class Component {
	public:
		enum Alignment {
			BOTTOM_ALIGNMENT, CENTER_ALIGNMENT, LEFT_ALIGNMENT, RIGHT_ALIGNMENT, TOP_ALIGNMENT
		};

		// Returns the alignment along the X axis. The return value = [0,1].
		virtual float getAlignmentX() const = 0;

		// Returns the alignment along the Y axis. The return value = [0,1].
		virtual float getAlignmentY() const = 0;

		// Returns the height.
		virtual float getHeight() const = 0;

		// Returns the width.
		virtual float getWidth() const = 0;

		// Returns the x position of the top left corner.
		virtual float getX() const = 0;

		// Returns the y position of the top left corner.
		virtual float getY() const = 0;		

		void addMouseListener() {
		}

		void addKeyListener() {
		}

		bool hasFocus() const {
		}

	private:
	};

	class Container : public Component {
	public:

	};

} // Namespace gui.

#endif // COMPONENT_H
