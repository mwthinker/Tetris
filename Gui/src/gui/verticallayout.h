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
			Point p = parent->getLocation();
			Dimension dimP = parent->getSize();
			p.y_ -= vFirstGap_;
			for (Component* c : *parent) {
				Dimension dim = c->getPreferredSize();
				c->setSize(dim);
				c->setLocation(p.x_ + hGap_, p.y_ + dimP.height_ - dim.height_);
				p.y_ -= dim.height_ + vGap_;
			}
		}

	private:
		float hGap_, vGap_, vFirstGap_;
	};

} // Namespace gui.

#endif // GUI_VERTICALLAYOUT_H
