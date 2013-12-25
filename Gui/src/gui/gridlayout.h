#ifndef GUI_GRIDLAYOUT_H
#define GUI_GRIDLAYOUT_H

#include "layoutmanager.h"
#include "panel.h"

#include <cassert>

namespace gui {
	
	class GridLayout : public LayoutManager {
	public:
		GridLayout(int rows, int columns) : columns_(columns), rows_(rows), hGap_(0), vGap_(0) {
		}

		GridLayout(int rows, int columns, float hGap, float vGap) : columns_(columns), rows_(rows), hGap_(hGap), vGap_(vGap) {
		}

		void layoutContainer(Panel* parent) override {
			Dimension dimP = parent->getSize();

			float w = dimP.width_ / columns_;
			float h = dimP.height_ / rows_;

			int row = 0;
			int column = 0;
			for (Component* c : *parent) {
				c->setLocation(column * (w + hGap_), row * (h + vGap_));
				c->setSize(w, h);

				++column;
				if (column >= columns_) {
					column = 0;
					++row;
				}
			}
		}

	private:
		int columns_, rows_;
		float hGap_, vGap_;
	};

} // Namespace gui.

#endif // GUI_GRIDLAYOUT_H
