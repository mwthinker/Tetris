#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "dimension.h"

namespace gui {	

	class Component;
	class Container;

	class LayoutManager {
	public:
		virtual Dimension getSize() const = 0;

		virtual Dimension preferredLayoutSize(Container* parent) const = 0;

		virtual void layoutContainer(Container* parent) = 0;
	};

} // Namespace gui.

#endif // LAYOUTMANAGER_H
