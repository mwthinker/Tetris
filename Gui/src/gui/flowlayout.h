#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include "layoutmanager.h"

#include <list>

namespace gui {

	class FlowLayout : public LayoutManager {
	public:
		enum Alignment {
			RIGHT
		};

		FlowLayout() {
			alignment_ = RIGHT;
		}

		FlowLayout(Alignment alignment) {
			alignment_ = alignment;
		}

		// Returns the height.
		float getHeight() const override {

		}

		// Returns the width.
		float getWidth() const override {

		}

		Dimension preferredLayoutSize(Container* parent) const override {

		}
		
		Dimension minimumLayoutSize(Container* parent) const override {

		}

		void layoutContainer(Container* parent) {
			switch (alignment_) {
				case RIGHT:
					layoutRight(parent);
					break;
			}
		}

	private:
		inline void layoutRight(Container* parent) {			

			for (Component* componenet : *parent) {
				Dimension dim = componenet->getPreferredSize();
			}
		}

		Alignment alignment_;
		int vGap_, hGap_;
	};

} // Namespace gui.

#endif // FLOWLAYOUT_H
