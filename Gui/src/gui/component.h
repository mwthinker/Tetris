#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include "dimension.h"

#include <mw/signal.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <sdl.h>

#include <stack>

namespace gui {

	class Component;

	using FocusListener = mw::Signal<Component*>;
	using KeyListener = mw::Signal<Component*, const SDL_Event&>;
	using MouseListener = KeyListener;
	using ActionListener = mw::Signal<Component*>;
	using PanelChangeListener = mw::Signal<Component*, bool>;

	class Component {
	public:
		friend class Panel;

		virtual ~Component() {
		}

		// Return the component's location in the parent's coordinate space.
		inline Point getLocation() const {
			return location_;
		}

		inline void setLocation(const Point& point) {
			location_ = point;
		}

		inline void setLocation(float x, float y) {
			location_ = Point(x, y);
		}

		void setPreferredSize(float width, float height);

		void setPreferredSize(const Dimension& dimension);

		inline Dimension getPreferredSize() const {
			return preferedDimension_;
		}

		// Return the components size.
		inline Dimension getSize() const {
			return dimension_;
		}

		inline void setSize(float width, float height) {
			dimension_ = Dimension(width, height);
		}

		inline void setSize(const Dimension& dimension) {
			dimension_ = dimension;
		}

		void setVisible(bool visible);

		inline bool isVisible() const {
			return visible_;
		}

		mw::signals::Connection addKeyListener(const KeyListener::Callback& callback);
		mw::signals::Connection addMouseListener(const MouseListener::Callback& callback);
		mw::signals::Connection addFocusListener(const FocusListener::Callback& callback);
		mw::signals::Connection addActionListener(const ActionListener::Callback& callback);
		mw::signals::Connection addPanelChangeListener(const PanelChangeListener::Callback& callback);

		// If set to true makes the component to always have focus. All parents will 
		// also be set to have focus. If the current state is the same as the change, 
		// nothing happens. If set to false, it will stop grabing focus unless there are 
		// children that want to grab focus.
		void setGrabFocus(bool grabFocus);

		bool isGrabFocus() const;

		// Sets the focus for the component.
		// Focuslistener is called if focus changes.
		virtual void setFocus(bool focus);

		// Return the focus for the component.
		bool hasFocus() const;

		inline Component* getParent() const {
			return parent_;
		}

		inline void setBackground(const mw::Sprite& background) {
			background_ = background;
		}

		inline const mw::Sprite& getBackground() const {
			return background_;
		}

		inline void setBackgroundColor(const mw::Color& color) {
			backgroundColor_ = color;
		}

		inline const mw::Color& getBackgroundColor() const {
			return backgroundColor_;
		}

		inline void setBorderColor(const mw::Color& color) {
			borderColor_ = color;
		}

		inline const mw::Color& getBorderColor() const {
			return borderColor_;
		}

		// Draws the gackground color.
		// Should be derived and should then draw the
		// component in the size defined by getSize().
		virtual void draw(Uint32 deltaTime);

		// Do action.
		void doAction();

		// Only called when the mouse leave the component.
		inline virtual void mouseMotionLeave() {
		}

		// Only called when the mouse up was inside the component.
		// And the up event was outside.
		inline virtual void mouseOutsideUp() {
		}

		virtual void panelChanged(bool active);

		// Gets the layout index for the component.
		// 0 is the default value.
		int getLayoutIndex() const;

		// Defines the layout for the component.
		// Must correspond to the active LayoutManager.
		void setLayoutIndex(int layoutIndex);

	protected:
		Component();

		// Takes care of all mouse events. And send it through to
		// all mouse listener callbacks.
		// Mouse events: SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN and SDL_MOUSEBUTTONUP.
		virtual void handleMouse(const SDL_Event& mouseEvent);

		// Takes care of all key events. And send it through to
		// all key listener callbacks.
		// Key events: SDL_TEXTINPUT, SDL_TEXTEDITING, SDL_KEYDOWN and SDL_KEYUP.
		virtual void handleKeyboard(const SDL_Event& keyEvent);

		// Fixes all child components sizes.
		// If there are no childs nothing happens.
		inline virtual void validate() {
		}

		// Is called in order to signal the parent component that
		// the childrens sizes must be recalculated.
		void validateParent();

		virtual void drawBorder();

	private:
		mw::Sprite background_;
		mw::Color backgroundColor_;
		mw::Color borderColor_;
		Component* parent_;
		Point location_;
		Dimension dimension_;
		Dimension preferedDimension_;
		int layoutIndex_;

		FocusListener focusListener_;
		KeyListener keyListener_;
		MouseListener mouseListener_;
		ActionListener actionListener_;
		PanelChangeListener panelChangeListener_;

		bool visible_;
		bool focus_;
		bool grabFocus_;
		int nbrChildGrabFocus_;

		bool isAdded_;
	};

} // Namespace gui.

#endif // GUI_COMPONENT_H
