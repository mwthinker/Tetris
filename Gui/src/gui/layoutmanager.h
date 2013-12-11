#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "dimension.h"

namespace gui {	

	class Component;
	class Container;

	class LayoutManager {
	public:
		// Returns the height.
		virtual float getHeight() const = 0;

		// Returns the width.
		virtual float getWidth() const = 0;

		virtual Dimension preferredLayoutSize(Container* parent) const = 0;
		virtual Dimension minimumLayoutSize(Container* parent) const = 0;

		virtual void layoutContainer(Container* parent) = 0;
	};

} // Namespace gui.

#endif // LAYOUTMANAGER_H
