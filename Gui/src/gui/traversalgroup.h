#ifndef TRAVERSALGROUP_H
#define TRAVERSALGROUP_H

#include "component.h"

#include <vector>

namespace gui {

	class TraversalGroup {
	public:
		TraversalGroup();

		void add(Component* component);

		void handleKeyboard(const SDL_Event& keyEvent);

		void sort();

	private:
		// Changes to next item if next is true else changes to the last item.
		void changeToNext(bool nextItem);

		int lastFocusIndex_;
		std::vector<Component*> components_;
	};

} // Namespace gui.

#endif // TRAVERSALGROUP_H
