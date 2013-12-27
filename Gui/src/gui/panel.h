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

		void validate() override;

		void setFocus(bool focus) override;

	private:
		typedef std::pair<Point, Dimension> Square;

		// Calls glScissor with the apropriated parameters based on the component
		// provided and the calls to glScissor earlier in in the panel hierarchi.
		static void pushScissor(Component* component);

		// Undo the last call to pushScissor.
		static void pullScissor();

		// Returns true if the squares intersect else it returns false.
		static bool isIntersecting(const Square& sq1, const Square& sq2);

		// Return the intersect of two squares. If there is no intersected square sq2 is returned.
		static Square calculateIntersectSquare(const Square& sq1, const Square& sq2);

		// The stack of the recent glscissor squares.
		static std::stack<Square> squares_;

		std::list<Component*> components_;
		LayoutManager* layoutManager_;

		Component* mouseMotionInsideComponent_;
		Component* mouseDownInsideComponent_;
	};

} // Namespace gui.

#endif // GUI_PANEL_H
