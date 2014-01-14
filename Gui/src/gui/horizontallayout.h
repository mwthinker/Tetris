#ifndef GUI_HORIZONTALLAYOUT_H
#define GUI_HORIZONTALLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <cassert>

namespace gui {

	class HorizontalLayout : public LayoutManager {
	public:
		enum Alignment {
			LEFT,
			RIGHT
		};

		HorizontalLayout(Alignment alignment = LEFT) : hGap_(5), vGap_(5), hFirstGap_(0) {
		}

		HorizontalLayout(float hGap, float vGap, float hFirstGap, Alignment alignment = LEFT)
			: alignment_(alignment), hGap_(hGap), vGap_(vGap), hFirstGap_(hFirstGap) {
		}

		void layoutContainer(Panel* parent) override {
			Dimension dimP = parent->getSize();
			float x = hFirstGap_;
			for (Component* c : *parent) {
				if (!c->isVisible()) {
					continue;
				}
				Dimension dim = c->getPreferredSize();
				c->setSize(dim);
				switch (alignment_) {
					case LEFT:
						c->setLocation(x, dimP.height_ - dim.height_);
						break;
					case RIGHT:
						c->setLocation(dimP.width_ - dim.width_ - x, dimP.height_ - dim.height_);
						break;
				}
				x += hGap_ + dim.width_;
			}
		}

	private:
		Alignment alignment_;
		float hGap_, vGap_, hFirstGap_;
	};

} // Namespace gui.

#endif // GUI_HORIZONTALLAYOUT_H
