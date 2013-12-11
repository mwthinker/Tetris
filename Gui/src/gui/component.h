#ifndef COMPONENT_H
#define COMPONENT_H

#include "layoutmanager.h"
#include "dimension.h"

#include <mw/signal.h>

#include <list>

namespace gui {

	class Component {
	public:
		// Returns the alignment along the X axis. The return value = [0,1].
		virtual float getAlignmentX() const = 0;

		// Returns the alignment along the Y axis. The return value = [0,1].
		virtual float getAlignmentY() const = 0;

		virtual void setPreferredSize(int width, int height);

		virtual Dimension getPreferredSize() const = 0;

		// Returns the size.
		virtual Dimension getSize() const = 0;

		// Returns the x position of the top left corner.
		virtual float getX() const = 0;

		// Returns the y position of the top left corner.
		virtual float getY() const = 0;

		virtual void setVisible(bool visible) = 0;

		virtual bool isVisible() const = 0;

		virtual void doLayout() {
		}

		bool isValid() const {
		}

		void addMouseListener() {
		}

		void addKeyListener() {
		}

		bool hasFocus() const {
		}

		Component* getParent() const {
			return parent_;
		}		

	private:
		Component* parent_;
	};

	class Container {
	public:
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

	private:
		std::list<Component*> components_;
		LayoutManager* layoutManager_;
	};

} // Namespace gui.

#endif // COMPONENT_H
