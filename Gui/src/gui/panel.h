#ifndef PANEL_H
#define PANEL_H

#include "component.h"

#include <list>

namespace gui {

	class LayoutManager;

	class Panel : public Component {
	public:
		friend class Frame;

		void add(Component* component);

		void setLayout(LayoutManager* layoutManager);

		LayoutManager* getLayout(LayoutManager* layoutManager);

		std::list<Component*>::iterator begin();

		std::list<Component*>::iterator end();

		std::list<Component*>::const_iterator begin() const;

		std::list<Component*>::const_iterator end() const;

		int nbrOfComponents() const;

		std::list<Component*> getComponents() const;

		void draw(float deltaTime) override;

	private:
		std::list<Component*> components_;
		LayoutManager* layoutManager_;
	};

} // Namespace gui.

#endif // PANEL_H
