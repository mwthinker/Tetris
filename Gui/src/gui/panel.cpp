#include "panel.h"
#include "component.h"
#include "flowlayout.h"

#include <list>
#include <cassert>

namespace gui {

	Panel::Panel() : mouseMotionInsideComponent_(false), mouseDownInsideComponent_(false), layoutManager_(nullptr) {
		setLayout(new FlowLayout);
		setPreferredSize(50, 50);
		setBorderColor(mw::Color(1, 1, 1, 0));
	}

	Panel::~Panel() {
		for (Component* c : *this) {
			delete c;
		}

		delete layoutManager_;
	}

	void Panel::add(Component* component) {
		// Is already added?
		assert(!component->isAdded_);
		component->isAdded_ = true;
		component->parent_ = this;
		component->setLayoutIndex(0);
		components_.push_back(component);
		if (component->isGrabFocus()) {
			setFocus(true);
			++nbrChildGrabFocus_;
		}
		validate();
	}

	void Panel::add(Component* component, int layoutIndex) {
		// Is already added?
		assert(!component->isAdded_);
		component->isAdded_ = true;
		component->parent_ = this;
		component->setLayoutIndex(layoutIndex);
		components_.push_back(component);
		if (component->isGrabFocus()) {
			setFocus(true);
			++nbrChildGrabFocus_;
		}
		validate();
	}

	void Panel::addToGroup(Component* component) {
		add(component);
		group_.add(component);
	}

	void Panel::addToGroup(Component* component, int layoutIndex) {
		add(component, layoutIndex);
		group_.add(component);
	}

	void Panel::setLayout(LayoutManager* layoutManager) {
		if (layoutManager != nullptr) {
			delete layoutManager_;
			layoutManager_ = layoutManager;
			validate();
		}
	}

	void Panel::setFocus(bool focus) {
		Component::setFocus(focus);
		if (!focus) {
			for (Component* component : *this) {
				component->setFocus(false);
			}
		}
	}

	LayoutManager* Panel::getLayout() const {
		return layoutManager_;
	}

	std::vector<Component*>::iterator Panel::begin() {
		return components_.begin();
	}

	std::vector<Component*>::iterator Panel::end() {
		return components_.end();
	}

	std::vector<Component*>::const_iterator Panel::cbegin() const {
		return components_.begin();
	}

	std::vector<Component*>::const_iterator Panel::cend() const {
		return components_.end();
	}

	int Panel::nbrOfComponents() const {
		return components_.size();
	}

	const std::vector<Component*>& Panel::getComponents() const {
		return components_;
	}

	void Panel::draw(Uint32 deltaTime) {
		pushScissor(this);
		Component::draw(deltaTime);

		// Draw components.
		for (Component* component : *this) {
			if (component->isVisible()) {
				glPushMatrix();
				Point p = component->getLocation();
				glTranslated(p.x_, p.y_, 0.f);
				component->draw(deltaTime);
				glPopMatrix();
			}
		}

		pullScissor();
	}

	// Todo! Reverse y-axis!
	void Panel::handleMouse(const SDL_Event& mouseEvent) {
		Component* currentComponent = nullptr;
		switch (mouseEvent.type) {
			case SDL_MOUSEMOTION:
				for (Component* component : *this) {
					if (!component->isVisible()) {
						continue;
					}
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
					if (!component->isVisible()) {
						continue;
					}
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
						if (!component->isVisible()) {
							continue;
						}
						if (currentComponent == component) {
							component->setFocus(true);
						} else {
							component->setFocus(false);
						}
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
		group_.handleKeyboard(keyEvent);
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

	void Panel::panelChanged(bool active) {
		if (active) {
			validate();
		}

		for (Component* component : *this) {
			component->panelChanged(active);
		}
		Component::panelChanged(active);
	}

	void Panel::validate() {
		// Validate!
		layoutManager_->layoutContainer(this);
		for (Component* child : *this) {
			child->validate();
		}

		group_.sort();
	}

	std::stack<Panel::Square> Panel::squares_;

	bool Panel::isIntersecting(const Square& sq1, const Square& sq2) {
		return (((sq1.first.x_ + +sq1.second.width_ < sq2.first.x_) ||
			(sq2.first.x_ + +sq2.second.width_ < sq1.first.x_)) &&
			((sq1.first.y_ + +sq1.second.height_ < sq2.first.y_) ||
			(sq2.first.y_ + +sq2.second.height_ < sq1.first.y_)));
	}

	Panel::Square Panel::calculateIntersectSquare(const Square& sq1, const Square& sq2) {
		float x, y, w, h;
		if (isIntersecting(sq1, sq2)) {
			return sq2;
		}

		if (sq1.first.x_ < sq2.first.x_) {
			x = sq2.first.x_;
			w = sq1.second.width_ - (sq2.first.x_ - sq1.first.x_);
			if (sq1.first.y_ < sq2.first.y_) {
				y = sq2.first.y_;
				h = sq1.second.height_ - (sq2.first.y_ - sq1.first.y_);
			} else {
				y = sq1.first.y_;
				h = sq2.second.height_ - (sq1.first.y_ - sq2.first.y_);
			}
		} else {
			x = sq1.first.x_;
			w = sq2.second.width_ - (sq1.first.x_ - sq2.first.x_);
			if (sq1.first.y_ < sq2.first.y_) {
				y = sq2.first.y_;
				h = sq1.second.height_ - (sq2.first.y_ - sq1.first.y_);
			} else {
				y = sq1.first.y_;
				h = sq2.second.height_ - (sq1.first.y_ - sq2.first.y_);
			}
		}
		return Square(Point(x, y), Dimension(w, h));
	}

	void Panel::pushScissor(Component* component) {
		/*
		glEnable(GL_SCISSOR_TEST);
		Point loc = component->location_;
		Dimension dim = component->dimension_;
		if (squares_.empty()) {
		squares_.push(Square(loc, dim));
		} else {
		squares_.push(calculateIntersectSquare(squares_.top(), Square(loc, dim)));
		}
		Square sq = squares_.top();

		glScissor((GLsizei) sq.first.x_, (GLsizei) sq.first.y_,
		(GLsizei) sq.second.width_, (GLsizei) sq.second.height_);
		squares_.empty();
		*/
	}

	void Panel::pullScissor() {
		/*
		glDisable(GL_SCISSOR_TEST);
		if (!squares_.empty()) {
		squares_.pop();
		}
		*/
	}

} // Namespace gui.
