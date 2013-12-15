#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <list>

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

		Dimension preferredLayoutSize(Panel* parent) const override {
			return Dimension();
		}

		void layoutContainer(Panel* parent) {
			switch (alignment_) {
				case LEFT:
					layoutLeft(parent);
					break;
			}
		}

	private:
		inline void layoutLeft(Panel* parent) {
			parent->setToValid();
			double yMean = 0;
			double xMean = 0;
			double length = 0;

			for (Component* component : *parent) {
				Dimension dim = component->getPreferredSize();
				yMean += dim.height_ / 2.0;
				xMean += dim.width_ / 2.0;
				length += dim.width_;
			}
			yMean /= parent->nbrOfComponents();
			xMean /= parent->nbrOfComponents();			

			float x = 0;
			float y = 0;
			for (Component* component : *parent) {
				component->setLocation(x, y);
				component->setSize(component->getPreferredSize());
				component->setToValid();
				x += component->getSize().width_;
				component->setToValid();
			}
		}

		Alignment alignment_;
		float vGap_, hGap_;
	};

} // Namespace gui.

#endif // FLOWLAYOUT_H
