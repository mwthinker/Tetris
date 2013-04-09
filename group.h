#ifndef GROUP_H
#define GROUP_H

#include "guiitem.h"
#include "button.h"

namespace gui {

	class Group : public GuiItem {
	public:
		Group(SDLKey last, SDLKey next) {
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
				SDLKey key = windowEvent.key.keysym.sym;
				if (last_ == key) {
					unsigned int index = lastFocusIndex_;
					for (unsigned int i = 0; i < items_.size(); ++i) {
						if (items_[i]->hasFocus()) {
							index = (i - 1 + items_.size()) % items_.size();
						}
					}
					for (unsigned int i = 0; i < items_.size(); ++i) {
						items_[i]->setFocus(false);
					}
					items_[index]->setFocus(true);
					lastFocusIndex_ = index;
				}
				if (next_ == key) {
					unsigned int index = lastFocusIndex_;
					for (unsigned int i = 0; i < items_.size(); ++i) {
						if (items_[i]->hasFocus()) {
							index = (i + 1) % items_.size();
						}				
					}
					for (unsigned int i = 0; i < items_.size(); ++i) {
						items_[i]->setFocus(false);
					}
					items_[index]->setFocus(true);
					lastFocusIndex_ = index;
				}
				if (SDLK_RETURN == key) {
					for (unsigned int i = 0; i < items_.size(); ++i) {
						if (items_[i]->hasFocus()) {
							items_[i]->click();
							break;
						}
					}
				}
			}
		}

	private:
		int lastFocusIndex_;

		SDLKey last_, next_;
		std::vector<GuiItemPtr> items_;
	};

	typedef std::shared_ptr<Group> GroupPtr;

} // Namespace gui.

#endif GROUP_H
