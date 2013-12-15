#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "dimension.h"

namespace gui {	

	class Panel;

	class LayoutManager {
	public:
		// Calculates the prefered size of the parent
		// based of the childs' sizes.
		virtual Dimension preferredLayoutSize(Panel* parent) const = 0;

		// Sets the size of childs based on the current
		// size of the parent.
		virtual void layoutContainer(Panel* parent) = 0;
	};

} // Namespace gui.

#endif // LAYOUTMANAGER_H
