#ifndef GUI_FLOWLAYOUT_H
#define GUI_FLOWLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <algorithm>
#include <cassert>

namespace gui {

	class FlowLayout : public LayoutManager {
	public:
		enum Alignment {
			LEFT,
			CENTER,
			RIGHT
		};

		FlowLayout() {
			alignment_ = LEFT;
			hGap_ = 5;
			vGap_ = 5;
		}

		FlowLayout(Alignment alignment) {
			alignment_ = alignment;
			hGap_ = 5;
			vGap_ = 5;
		}

		FlowLayout(Alignment alignment, float hGap, float vGap) {
			alignment_ = alignment;
			hGap_ = hGap;
			vGap_ = vGap;
		}

		void layoutContainer(Panel* parent) override {
			Dimension dimP = parent->getSize();

			float w = hGap_;
			float h = 0;
			float maxH = 0;
			for (Component* c : *parent) {
				Dimension dimC = c->getPreferredSize();
				c->setSize(dimC);
				if (!c->isVisible()) {
					continue;
				}
				if (w + dimC.width_ >= dimP.width_) {
					w = 0;
					h += maxH + vGap_;
					maxH = 0;
				}
				maxH = std::max(maxH, dimC.height_);

				switch (alignment_) {
					case LEFT:
						c->setLocation(w, dimP.height_ - h - dimC.height_);
						break;
					case CENTER:
						// Todo!
						//c->setLocation((dimP.width_ + dimC.width_ - w) * 0.5f, dimP.height_ - h - dimC.height_);
						break;
					case RIGHT:
						c->setLocation(dimP.width_ - w - dimC.width_, dimP.height_ - h - dimC.height_);
						break;
				}
				w += dimC.width_ + hGap_;
			}
		}

	private:
		Alignment alignment_;
		float hGap_, vGap_;
	};

} // Namespace gui.

#endif // GUI_FLOWLAYOUT_H
