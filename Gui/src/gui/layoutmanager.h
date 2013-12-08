#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

namespace gui {	

	class Component;

	class LayoutManager {
	public:
		virtual void add(Component* component) = 0;
	};

} // Namespace gui.

#endif // LAYOUTMANAGER_H
