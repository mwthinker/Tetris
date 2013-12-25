#ifndef GUI_VERTICALLAYOUT_H
#define GUI_VERTICALLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <cassert>

namespace gui {

	class VerticalLayout : public LayoutManager {
	public:
		VerticalLayout() : hGap_(5), vGap_(5), vFirstGap_(0) {
		}

		VerticalLayout(float hGap, float vGap, float vFirstGap) : hGap_(hGap), vGap_(vGap), vFirstGap_(vFirstGap) {
		}

		void layoutContainer(Panel* parent) override {			

			Dimension dimP = parent->getSize();
			float x = 0;
			float y = -vFirstGap_;
			for (Component* c : *parent) {
				Dimension dim = c->getPreferredSize();
				c->setSize(dim);
				c->setLocation(x + hGap_, y + dimP.height_ - dim.height_);
				y -= dim.height_ + vGap_;
			}
		}

	private:
		float hGap_, vGap_, vFirstGap_;
	};

} // Namespace gui.

#endif // GUI_VERTICALLAYOUT_H
