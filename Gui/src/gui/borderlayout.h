#ifndef GUI_BORDERLAYOUT_H
#define GUI_BORDERLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

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

		void layoutContainer(Panel* parent) override {
			float n, s, w, e;
			calculateSizes(parent, n, s, w, e);

			Dimension dimP = parent->getSize();
			for (Component* c : *parent) {
				if (!c->isVisible()) {
					continue;
				}
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
					default:
						// Error.
						assert(0);
						break;
				}
			}
		}

	private:
		inline void calculateSizes(Panel* parent, float& north, float& south, float& west, float& east) const {
			north = 0;
			south = 0;
			west = 0;
			east = 0;
			for (Component* c : *parent) {
				if (!c->isVisible()) {
					continue;
				}

				switch (c->getLayoutIndex()) {
					case CENTER:
						// Maximize.
						break;
					case NORTH:
						north = c->getPreferredSize().height_;
						break;
					case SOUTH:
						south = c->getPreferredSize().height_;
						break;
					case WEST:
						west = c->getPreferredSize().width_;
						break;
					case EAST:
						east = c->getPreferredSize().width_;
						break;
					default:
						// Error.
						assert(0);
						break;
				}
			}
		}
	};

} // Namespace gui.

#endif // GUI_BORDERLAYOUT_H
