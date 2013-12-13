#ifndef COMPONENT_H
#define COMPONENT_H

#include "layoutmanager.h"
#include "dimension.h"

#include <mw/signal.h>
#include <mw/color.h>

#include <sdl.h>

#include <list>

namespace gui {

	class Component;

	using FocusListener = mw::Signal<Component*, const SDL_Event&>;
	using KeyListener = FocusListener;
	using MouseListener = KeyListener;

	class Component {
	public:
		void setLocation(float x, float y) {
			location_ = Point(x, y);
		}

		// Returns the alignment along the Y and X axis. The return value = [0,1].
		Dimension getAlignment() const {
			return alignment_;
		}

		void setPreferredSize(float width, float height) {
			preferedDimension_ = Dimension(width, height);
		}

		Dimension getPreferredSize() const {
			return preferedDimension_;
		}

		// Returns the size.
		Dimension getSize() const {
			return dimension_;
		}

		void setSize(float width, float height) {
			dimension_ = Dimension(width, height);
		}

		void setVisible(bool visible) {
			visible_ = visible;
		}

		bool isVisible() const {
			return visible_;
		}

		bool isValid() const {
			return valid_;
		}

		mw::signals::Connection addKeyListener(const KeyListener::Callback& callback) {
			return keyListener_.connect(callback);
		}

		mw::signals::Connection addMouseListener(const MouseListener::Callback& callback) {
			return mouseListener.connect(callback);
		}

		mw::signals::Connection addFocusListener(const FocusListener::Callback& callback) {
			return focusListener_.connect(callback);
		}

		bool hasFocus() const {
			return focus_;
		}

		Component* getParent() const {
			return parent_;
		}

		float leftBorder() const {
			return 5;
		}

		float rightBorder() const {
			return 5;
		}

		float upBorder() const {
			return 5;
		}

		float downBorder() const {
			return 5;
		}

		void setBackground(const mw::Color& color) {
			background_ = color;
		}

		virtual void draw(float deltaTime) {
		}

	protected:
		mw::Color background_;
		Component* parent_;
		Point location_;
		Dimension dimension_;
		Dimension preferedDimension_;
		Dimension alignment_;

		FocusListener focusListener_;
		KeyListener keyListener_;
		MouseListener mouseListener;

		bool visible_;
		bool valid_;
		bool focus_;
	};

	class Container : public Component {
	public:
		friend class Frame;

		void add(Component* component) {
			components_.push_back(component);
		}

		void setLayout(LayoutManager* layoutManager) {
			layoutManager_ = layoutManager;
		}

		LayoutManager* getLayout(LayoutManager* layoutManager) {
			return layoutManager_;
		}

		std::list<Component*>::iterator begin() {
			return components_.begin();
		}

		std::list<Component*>::iterator end() {
			return components_.end();
		}

		std::list<Component*>::const_iterator begin() const {
			return components_.begin();
		}

		std::list<Component*>::const_iterator end() const {
			return components_.end();
		}

		int nbrOfComponents() const {
			return components_.size();
		}

		std::list<Component*> getComponents() const {
			return components_;
		}

	private:
		std::list<Component*> components_;
		LayoutManager* layoutManager_;
	};

} // Namespace gui.

#endif // COMPONENT_H
