#ifndef BORDERLAYOUT_H
#define BORDERLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <list>
#include <cassert>

namespace gui {

	class BorderLayout : public LayoutManager {
	public:
		static const int CENTER = 0;
		static const int NORTH = 1;
		static const int WEST = 2;
		static const int EAST = 3;
		static const int SOUTH = 4;

		BorderLayout() {
		}

		BorderLayout(float vGap, float hGap) {
			vGap_ = vGap;
			hGap_ = hGap;
		}
		
		Dimension preferredLayoutSize(Panel* parent) const override {
			return Dimension();
		}

		void layoutContainer(Panel* parent) {
			parent->setToValid();
			float n = 0;
			float s = 0;
			float w = 0;
			float e = 0;
			for (Component* c : *parent) {
				switch (c->getLayoutIndex()) {
					case CENTER:
						// Maximize.
						break;
					case NORTH:
						n = c->getPreferredSize().height_;
						break;
					case SOUTH:
						s = c->getPreferredSize().height_;
						break;
					case WEST:
						w = c->getPreferredSize().width_;
						break;
					case EAST:
						e = c->getPreferredSize().width_;
						break;
				}
			}

			Dimension dimP = parent->getPreferredSize();
			for (Component* c : *parent) {
				Dimension dimC = c->getPreferredSize();
				switch (c->getLayoutIndex()) {
					case CENTER:
						c->setSize(dimP.width_ - w - e, dimP.height_ - n - s);
						c->setLocation(w, s);
						break;
					case NORTH:
						c->setSize(dimP.width_, n);
						c->setLocation(0, dimP.height_ - n);
						break;
					case SOUTH:
						c->setSize(dimP.width_, s);
						c->setLocation(0, 0);
						break;
					case WEST:
						c->setSize(w, dimP.height_ - n - s);
						c->setLocation(0, s);
						break;
					case EAST:
						c->setSize(e, dimP.height_ - n - s);
						c->setLocation(dimP.width_ - e, s);
						break;
				}
				c->setToValid();
			}
		}

	private:
		float vGap_, hGap_;
	};

} // Namespace gui.

#endif // BORDERLAYOUT_H
