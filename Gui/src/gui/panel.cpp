#include "panel.h"
#include "component.h"
#include "layoutmanager.h"

#include <list>

namespace gui {

	void Panel::add(Component* component) {
		component->parent_ = this;
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
		
		if (!isValid()) {
			// Validate!
			layoutManager_->layoutContainer(this);
		}

		// Draw panel background.		
		getBackground().glColor3d();
		Dimension dim = getSize();
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(dim.width_*1.0f, 0.0f, 0.0f);
		glVertex3f(dim.width_*1.0f, dim.height_*1.0f, 0.0f);
		glVertex3f(0.0f, dim.height_*1.0f, 0.0f);
		glEnd();

		if (!isValid()) {
			layoutManager_->layoutContainer(this);
		}

		// Draw components.
		for (Component* component : *this) {
			glPushMatrix();
			Point p = component->getLocation();
			glTranslated(p.x_, p.y_, 0.f);
			component->draw(deltaTime);
			glPopMatrix();
		}
	}

	// Todo! Reverse y-axis!
	void Panel::handleMouse(const SDL_Event& mouseEvent) {
		switch (mouseEvent.type) {			
			case SDL_MOUSEMOTION:
				for (Component* component : *this) {
					Point p = component->getLocation();
					Dimension d = component->getSize();
					if (p.x_ <= mouseEvent.motion.x && p.x_ + d.width_ > mouseEvent.motion.x &&
						p.y_ <= mouseEvent.motion.y && p.y_ + d.height_ > mouseEvent.motion.y) {
						SDL_Event motionEvent = mouseEvent;
						motionEvent.motion.x -= p.x_;
						motionEvent.motion.y -= p.y_;
						component->handleMouse(motionEvent);
					}
				}
				Component::handleMouse(mouseEvent);
				break;
			case SDL_MOUSEBUTTONDOWN:
				// Fall through!
			case SDL_MOUSEBUTTONUP:
				for (Component* component : *this) {
					Point p = component->getLocation();
					Dimension d = component->getSize();
					if (p.x_ <= mouseEvent.button.x && p.x_ + d.width_ > mouseEvent.button.x &&
						p.y_ <= mouseEvent.button.y && p.y_ + d.height_ > mouseEvent.button.y) {
						SDL_Event buttonEvent = mouseEvent;
						buttonEvent.motion.x -= p.x_;
						buttonEvent.motion.y -= p.y_;
						component->handleMouse(buttonEvent);
					}
				}
				Component::handleMouse(mouseEvent);
				break;
		}
	}

	void Panel::handleKeyboard(const SDL_Event&) {

	}

} // Namespace gui.
