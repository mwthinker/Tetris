#include "panel.h"
#include "component.h"
#include "flowlayout.h"

#include <list>

namespace gui {

	Panel::Panel() : mouseMotionInsideComponent_(false), mouseDownInsideComponent_(false), layoutManager_(nullptr) {
		setLayout(new FlowLayout);
	}

	Panel::~Panel() {
		for (Component* c : *this) {
			delete c;
		}

		delete layoutManager_;
	}
	
	void Panel::add(Component* component) {
		component->parent_ = this;
		component->setLayoutIndex(0);
		components_.push_back(component);
	}

	void Panel::add(Component* component, int layoutIndex) {
		component->parent_ = this;
		component->setLayoutIndex(layoutIndex);
		components_.push_back(component);
	}

	void Panel::setLayout(LayoutManager* layoutManager) {
		if (layoutManager != nullptr) {
			delete layoutManager_;
			layoutManager_ = layoutManager;
		}
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

	std::list<Component*>::const_iterator Panel::cbegin() const {
		return components_.begin();
	}

	std::list<Component*>::const_iterator Panel::cend() const {
		return components_.end();
	}

	int Panel::nbrOfComponents() const {
		return components_.size();
	}

	const std::list<Component*>& Panel::getComponents() const {
		return components_;
	}

	void Panel::draw(float deltaTime) {
		if (!isValid()) {
			// Validate!
			layoutManager_->layoutContainer(this);
		}

		Component::draw(deltaTime);

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
		Component* currentComponent = nullptr;
		switch (mouseEvent.type) {
			case SDL_MOUSEMOTION:
				for (Component* component : *this) {
					Point p = component->getLocation();
					Dimension d = component->getSize();
					if (p.x_ <= mouseEvent.motion.x && p.x_ + d.width_ > mouseEvent.motion.x &&
						p.y_ <= mouseEvent.motion.y && p.y_ + d.height_ > mouseEvent.motion.y) {
						SDL_Event motionEvent = mouseEvent;
						motionEvent.motion.x -= (Sint32) p.x_;
						motionEvent.motion.y -= (Sint32) p.y_;
						component->handleMouse(motionEvent);
						currentComponent = component;
						break;
					}
				}
				if (mouseMotionInsideComponent_ != currentComponent) {
					if (mouseMotionInsideComponent_ != nullptr) {
						mouseMotionInsideComponent_->mouseMotionLeave();
					}
				}
				mouseMotionInsideComponent_ = currentComponent;
				Component::handleMouse(mouseEvent);
				break;
			case SDL_MOUSEBUTTONDOWN:
				// Fall through!
			case SDL_MOUSEBUTTONUP:
				// Send the mouseEvent through to the correct component.
				for (Component* component : *this) {
					Point p = component->getLocation();
					Dimension d = component->getSize();

					// Mouse is inside the component?
					if (p.x_ <= mouseEvent.button.x && p.x_ + d.width_ > mouseEvent.button.x &&
						p.y_ <= mouseEvent.button.y && p.y_ + d.height_ > mouseEvent.button.y) {
						SDL_Event buttonEvent = mouseEvent;
						buttonEvent.motion.x -= (Sint32) p.x_;
						buttonEvent.motion.y -= (Sint32) p.y_;
						currentComponent = component;
						component->handleMouse(buttonEvent);
						break; // Abort, components should not overlap!
					}
				}

				if (mouseEvent.type == SDL_MOUSEBUTTONDOWN) {
					// Set all components focus to false except
					// the component used.					
					for (Component* component : *this) {
						component->setFocus(false);
					}
					if (currentComponent != nullptr) {
						currentComponent->setFocus(true);
					}
				}

				// Call the component if it was pushed and released outside
				// the component.
				if (mouseEvent.type == SDL_MOUSEBUTTONUP) {
					if (mouseDownInsideComponent_ != nullptr && mouseDownInsideComponent_ != currentComponent) {
						mouseDownInsideComponent_->mouseOutsideUp();
						mouseDownInsideComponent_ = nullptr;
					}
				} else if (mouseEvent.type == SDL_MOUSEBUTTONDOWN) {
					mouseDownInsideComponent_ = currentComponent;
				}

				Component::handleMouse(mouseEvent);
				break;
			default:
				break;
		}
	}

	void Panel::handleKeyboard(const SDL_Event& keyEvent) {
		for (Component* component : *this) {
			if (component->hasFocus()) {
				component->handleKeyboard(keyEvent);
			}
		}
		Component::handleKeyboard(keyEvent);
	}

	void Panel::mouseMotionLeave() {
		for (Component* component : *this) {
			component->mouseMotionLeave();
		}
	}

	void Panel::mouseOutsideUp() {
		for (Component* component : *this) {
			component->mouseOutsideUp();
		}
	}

} // Namespace gui.
