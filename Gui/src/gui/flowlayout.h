#ifndef GUI_FLOWLAYOUT_H
#define GUI_FLOWLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <list>
#include <algorithm>

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
			vGap_ = 5.f;
			hGap_ = 5.f;
		}

		FlowLayout(Alignment alignment) {
			alignment_ = alignment;
			vGap_ = 5.f;
			hGap_ = 5.f;
		}

		FlowLayout(Alignment alignment, float vGap, float hGap) {
			alignment_ = alignment;
			vGap_ = vGap;
			hGap_ = hGap;
		}

		void layoutContainer(Panel* parent) {
			parent->setToValid();
			Dimension dimP = parent->getPreferredSize();

			float w = hGap_;
			float h = vGap_;
			float maxH = 0;
			for (Component* c : *parent) {
				Dimension dimC = c->getPreferredSize();
				c->setSize(dimC);
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
				c->setToValid();
			}
		}

	private:
		Alignment alignment_;
		float vGap_, hGap_;
	};

} // Namespace gui.

#endif // GUI_FLOWLAYOUT_H
