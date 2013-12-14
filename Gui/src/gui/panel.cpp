#include "panel.h"
#include "component.h"
#include "layoutmanager.h"

#include <list>

namespace gui {

	void Panel::add(Component* component) {
		components_.push_back(component);
	}

	void Panel::setLayout(LayoutManager* layoutManager) {
		layoutManager_ = layoutManager;
	}

	LayoutManager* Panel::getLayout(LayoutManager* layoutManager) {
		return layoutManager_;
	}

	std::list<Component*>::iterator Panel::begin() {
		return components_.begin();
	}

	std::list<Component*>::iterator Panel::end() {
		return components_.end();
	}

	std::list<Component*>::const_iterator Panel::begin() const {
		return components_.begin();
	}

	std::list<Component*>::const_iterator Panel::end() const {
		return components_.end();
	}

	int Panel::nbrOfComponents() const {
		return components_.size();
	}

	std::list<Component*> Panel::getComponents() const {
		return components_;
	}

	void Panel::draw(float deltaTime) {
		getBackground().glColor3d();
		Dimension dim = getSize();
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(dim.width_*1.0f, 0.0f, 0.0f);
		glVertex3f(dim.width_*1.0f, dim.height_*1.0f, 0.0f);
		glVertex3f(0.0f, dim.height_*1.0f, 0.0f);
		glEnd();
	}

} // Namespace gui.
