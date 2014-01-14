#ifndef GUI_VERTICALLAYOUT_H
#define GUI_VERTICALLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <cassert>

namespace gui {

	class VerticalLayout : public LayoutManager {
	public:
		enum Alignment {
			UP,
			DOWN
		};

		VerticalLayout(Alignment alignment = UP) : alignment_(alignment), hGap_(5), vGap_(5), vFirstGap_(0) {
		}

		VerticalLayout(float hGap, float vGap, float vFirstGap, Alignment alignment = UP) 
			: alignment_(alignment), hGap_(hGap), vGap_(vGap), vFirstGap_(vFirstGap) {
		}

		void layoutContainer(Panel* parent) override {
			Dimension dimP = parent->getSize();
			float y = vFirstGap_;
			for (Component* c : *parent) {
				if (!c->isVisible()) {
					continue;
				}
				Dimension dim = c->getPreferredSize();
				c->setSize(dim);
				switch (alignment_) {
					case UP:
						c->setLocation(hGap_, dimP.height_ - dim.height_ - y);
						break;
					case DOWN:
						c->setLocation(hGap_, y);
						break;
				}
				y += vGap_ + dim.height_;
			}
		}

	private:
		Alignment alignment_;
		float hGap_, vGap_, vFirstGap_;
	};

} // Namespace gui.

#endif // GUI_VERTICALLAYOUT_H
