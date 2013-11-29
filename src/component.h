#ifndef COMPONENT_H
#define COMPONENT_H

#include <mw/signal.h>

namespace gui {

	class Component {
	public:
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

		// Returns true if the mouse is inside the Component else it returns false.
		// The mouse position is saved in x and y in Component's coordinate space.
		bool mousePosition(float& x, float& y) const {
			return true;
		}

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

	class LayoutManager {
	public:
		virtual void add(Component* component) = 0;
	};

} // Namespace gui.

#endif // COMPONENT_H
