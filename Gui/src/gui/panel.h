#ifndef GUI_PANEL_H
#define GUI_PANEL_H

#include "component.h"

#include <list>

namespace gui {

	class LayoutManager;

	// Creates a panel able to contain other components. It manage the
	// the layout of the components based on the layout manager.
	class Panel : public Component {
	public:
		friend class Frame;

		// Creates a empty panel. The default LayoutManager is FlowLayout.
		Panel();

		// Deallocats all contained components.
		~Panel();

		// Adds the component to the container. Takes the ownership.
		// I.e. Deallocates the component when the panel is deallocated.
		void add(Component* component);

		// Adds the component to the container using the layout spcified 
		// by the layout manager and the layoutIndex. Takes the ownership.
		// I.e. Deallocates the component when the panel is deallocated.
		void add(Component* component, int layoutIndex);

		// Sets the layouyt manager. Takes ower the ownership of the layoutManager.
		// The old layoutManager are dealloted.
		void setLayout(LayoutManager* layoutManager);
		
		// Gets the current layout manager. Do not deallocate the layout manager
		// the panel takes care of that!
		LayoutManager* getLayout() const;

		std::list<Component*>::iterator begin();
		std::list<Component*>::iterator end();
		std::list<Component*>::const_iterator cbegin() const;
		std::list<Component*>::const_iterator cend() const;

		// Returns the number of components contained.
		int nbrOfComponents() const;

		// Gets the list holding all contained components.
		const std::list<Component*>& getComponents() const;

		void draw(float deltaTime) override;

		void handleMouse(const SDL_Event& mouseEvent) override;

		void handleKeyboard(const SDL_Event&) override;
		
		void mouseMotionLeave() override;

		void mouseOutsideUp() override;

		void panelChanged(bool active) override;

	private:
		std::list<Component*> components_;
		LayoutManager* layoutManager_;

		Component* mouseMotionInsideComponent_;
		Component* mouseDownInsideComponent_;
	};

} // Namespace gui.

#endif // GUI_PANEL_H
