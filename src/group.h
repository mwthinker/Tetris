#ifndef GROUP_H
#define GROUP_H

#include "guitypedefs.h"

#include <memory>

namespace gui {

	class Group : public GuiItem {
	public:
		Group(SDL_Keycode last, SDL_Keycode next) {
			last_ = last;
			next_ = next;
			lastFocusIndex_ = 0;
		}

		void add(GuiItemPtr item) {
			items_.push_back(item);			
		}

		void eventUpdate(const SDL_Event& windowEvent, int x, int y) override {
			switch (windowEvent.type) {
			case SDL_KEYDOWN:
				{
					SDL_Keycode key = windowEvent.key.keysym.sym;
					if (last_ == key) {
						changeToNext(false);
					} else if (next_ == key) {
						changeToNext(true);
					} else if (SDLK_RETURN == key) {
						for (const GuiItemPtr& item : items_) {
							if (item->hasFocus() && item->isVisible()) {
								item->click();
								break;
							}
						}
					}
					break;
				}
			default:
				break;
			}
		}

	private:
		// Changes to next item if next is true else changes to the last item.
		void changeToNext(bool nextItem) {
			bool oneAtLeastVisible = false;
			for (const GuiItemPtr& item : items_) {
				if (item->isVisible()) {
					oneAtLeastVisible = true;
					break;
				}
			}
			if (oneAtLeastVisible) {
				unsigned int index = lastFocusIndex_;
				bool findFocusedItem = false;
				
				for (unsigned int i = 0; i < items_.size(); ++i) {
					if (nextItem) {
						index = (lastFocusIndex_ + i + 1) % items_.size();						
					} else {
						index = (lastFocusIndex_ - i - 1 + items_.size()) % items_.size();
					}
					if (items_[index]->isVisible()) {
						findFocusedItem = true;
						break;
					}
				}
			
				// At least one unit has focus?
				if (findFocusedItem) {
					// Take focus from all items.
					for (const GuiItemPtr& item : items_) {
						item->setFocus(false);
					}
					// Give focus to next item.
					if (items_[index]->isVisible()) {
						items_[index]->setFocus(true);
						lastFocusIndex_ = index;
					}
				} else {
					// Find first visible item and give focus.
					for (const GuiItemPtr& item : items_) {
						if (item->isVisible()) {
							item->setFocus(true);
							break;
						}
					}
				}
			}
		}

		int lastFocusIndex_;

		SDL_Keycode last_, next_;
		std::vector<GuiItemPtr> items_;
	};

	typedef std::shared_ptr<Group> GroupPtr;

	GroupPtr createGroup(SDL_Keycode last, SDL_Keycode next) {
		return GroupPtr(new Group(last,next));
	}

} // Namespace gui.

#endif // GROUP_H
