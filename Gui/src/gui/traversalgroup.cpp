#include "traversalgroup.h"

#include <algorithm>

namespace gui {

	TraversalGroup::TraversalGroup() {
		lastFocusIndex_ = 0;
	}

	void TraversalGroup::add(Component* component) {
		components_.push_back(component);
	}

	void TraversalGroup::handleKeyboard(const SDL_Event& keyEvent) {
		switch (keyEvent.type) {
			case SDL_KEYDOWN:
				switch (keyEvent.key.keysym.sym) {
					case SDLK_TAB:
					{
						const Uint8* state = SDL_GetKeyboardState(0);
						if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) {
							changeToNext(false);
						} else {
							changeToNext(true);
						}
						break;
					}
					case SDLK_DOWN:
						changeToNext(true);
						break;
					case SDLK_UP:
						changeToNext(false);
						break;
					case SDLK_RETURN:
						// Fall through!
					case SDLK_KP_ENTER:
						for (Component* c : components_) {
							if (c->hasFocus() && c->isVisible()) {
								c->doAction();
								break;
							}
						}
						break;
					default:
						break;

				}
			default:
				break;
		}
	}

	void TraversalGroup::sort() {
		std::sort(components_.rbegin(), components_.rend(), [](Component* c1, Component* c2) {
			Point p1 = c1->getLocation();
			Point p2 = c2->getLocation();

			if (p1.y_ == p2.y_) {
				return p1.x_ < p2.x_;
			}

			return p1.y_ < p2.y_;
		});
	}

	void TraversalGroup::changeToNext(bool nextItem) {
		bool oneAtLeastVisible = false;
		for (Component* c : components_) {
			if (c->isVisible()) {
				oneAtLeastVisible = true;
				break;
			}
		}
		if (oneAtLeastVisible) {
			unsigned int index = lastFocusIndex_;
			bool findFocusedItem = false;

			for (unsigned int i = 0; i < components_.size(); ++i) {
				if (nextItem) {
					index = (lastFocusIndex_ + i + 1) % components_.size();
				} else {
					index = (lastFocusIndex_ - i - 1 + components_.size()) % components_.size();
				}
				if (components_[index]->isVisible()) {
					findFocusedItem = true;
					break;
				}
			}

			// At least one unit has focus?
			if (findFocusedItem) {
				// Take focus from all items.
				for (Component* c : components_) {
					c->setFocus(false);
				}
				// Give focus to next item.
				if (components_[index]->isVisible()) {
					components_[index]->setFocus(true);
					lastFocusIndex_ = index;
				}
			} else {
				// Find first visible item and give focus.
				for (Component* c : components_) {
					if (c->isVisible()) {
						c->setFocus(true);
						break;
					}
				}
			}
		}
	}

} // Namespace gui.
