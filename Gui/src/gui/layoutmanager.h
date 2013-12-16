#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "dimension.h"

namespace gui {	

	class Panel;

	class LayoutManager {
	public:
		virtual ~LayoutManager() {
		}

		// Sets the size of childs based on the current
		// size of the parent.
		virtual void layoutContainer(Panel* parent) = 0;
	};

} // Namespace gui.

#endif // LAYOUTMANAGER_H
