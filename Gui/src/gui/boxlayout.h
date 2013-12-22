#ifndef GUI_BOXLAYOUT_H
#define GUI_BOXLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <cassert>

namespace gui {

	class BoxLayout : public LayoutManager {
	public:
		enum Direction {
			HORIZONTAL,
			VERTICAL
		};

		BoxLayout() {
		}

		void layoutContainer(Panel* parent) override {
		}

	private:
	};

} // Namespace gui.

#endif // GUI_BOXLAYOUT_H
