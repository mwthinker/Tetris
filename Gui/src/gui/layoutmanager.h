#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "dimension.h"

namespace gui {	

	class Panel;

	class LayoutManager {
	public:
		virtual Dimension getSize() const = 0;

		virtual Dimension preferredLayoutSize(Panel* parent) const = 0;

		virtual void layoutContainer(Panel* parent) = 0;
	};

} // Namespace gui.

#endif // LAYOUTMANAGER_H
