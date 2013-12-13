#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include "layoutmanager.h"

#include <list>

namespace gui {

	class FlowLayout : public LayoutManager {
	public:
		enum Alignment {
			LEFT
		};

		FlowLayout() {
			alignment_ = LEFT;
		}

		FlowLayout(Alignment alignment) {
			alignment_ = alignment;
		}

		Dimension getSize() const override {
			return dimension_;
		}

		Dimension preferredLayoutSize(Container* parent) const override {
			return dimension_;
		}		

		void layoutContainer(Container* parent) {
			switch (alignment_) {
				case LEFT:
					layoutLeft(parent);
					break;
			}
		}

	private:
		inline void layoutLeft(Container* parent) {
			double yMean = 0;
			double xMean = 0;
			double length = 0;

			for (Component* component : *parent) {
				Dimension dim = component->getPreferredSize();
				yMean += dim.height_ / 2.0;
				xMean += dim.width_ / 2.0;
				length += component->leftBorder() + component->rightBorder() + dim.width_;
			}
			yMean /= parent->nbrOfComponents();
			xMean /= parent->nbrOfComponents();

			for (Component* component : *parent) {
				component->setLocation(component->leftBorder(), component->downBorder());
			}
		}

		Alignment alignment_;
		int vGap_, hGap_;
		Dimension dimension_;
	};

} // Namespace gui.

#endif // FLOWLAYOUT_H
