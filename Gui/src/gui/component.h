#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include "dimension.h"

#include <mw/signal.h>
#include <mw/color.h>
#include <mw/texture.h>

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
		friend class Frame;

		virtual ~Component() {
		}

		// Return the component's location in the parent's coordinate space.
		Point getLocation() const {
			return location_;
		}

		void setLocation(float x, float y) {
			location_ = Point(x, y);
		}

		// Returns the alignment along the Y and X axis. The return value = [0,1].
		Dimension getAlignment() const {
			return alignment_;
		}

		void setPreferredSize(float width, float height) {
			preferedDimension_ = Dimension(width, height);
			validateParent();
		}

		void setPreferredSize(const Dimension& dimension) {
			preferedDimension_ = dimension;
			validateParent();
		}

		Dimension getPreferredSize() const {
			return preferedDimension_;
		}

		// Returns the components size.
		Dimension getSize() const {
			return dimension_;
		}

		void setSize(float width, float height) {
			dimension_ = Dimension(width, height);
		}

		void setSize(const Dimension& dimension) {
			dimension_ = dimension;
		}

		void setVisible(bool visible) {
			visible_ = visible;
		}

		bool isVisible() const {
			return visible_;
		}

		mw::signals::Connection addKeyListener(const KeyListener::Callback& callback) {
			return keyListener_.connect(callback);
		}

		mw::signals::Connection addMouseListener(const MouseListener::Callback& callback) {
			return mouseListener_.connect(callback);
		}

		mw::signals::Connection addFocusListener(const FocusListener::Callback& callback) {
			return focusListener_.connect(callback);
		}

		mw::signals::Connection addActionListener(const ActionListener::Callback& callback) {
			return actionListener_.connect(callback);
		}

		mw::signals::Connection addPanelChangeListener(const PanelChangeListener::Callback& callback) {
			return panelChangeListener_.connect(callback);
		}

		// Sets the focus for the component.
		// Focuslistener is called if focus changes.
		virtual void setFocus(bool focus) {
			if (focus != focus_) {
				focus_ = focus;
				focusListener_(this);
			}
		}

		// Return the focus for the component.
		bool hasFocus() const {
			return focus_;
		}

		Component* getParent() const {
			return parent_;
		}
		
		void setBackground(const mw::TexturePtr& background) {
			background_ = background;
		}

		inline mw::TexturePtr getBackground() const {
			return background_;
		}

		void setBackgroundColor(const mw::Color& color) {
			backgroundColor_ = color;
		}

		inline const mw::Color& getBackgroundColor() const {
			return backgroundColor_;
		}

		// Draws the gackground color.
		// Should be derived and should then draw the
		// component in the size defined by getSize().
		virtual void draw(float deltaTime);

		// Do action.
		void doAction() {
			actionListener_(this);
		}

		// Only called when the mouse leave the component.
		virtual void mouseMotionLeave() {
		}

		// Only called when the mouse up was inside the component.
		// And the up event was outside.
		virtual void mouseOutsideUp() {
		}

		virtual void panelChanged(bool active) {
			panelChangeListener_(this, active);
		}

		// Gets the layout index for the component.
		// 0 is the default value.
		int getLayoutIndex() const {
			return layoutIndex_;
		}
		
		// Defines the layout for the component.
		// Must correspond to the active LayoutManager.
		void setLayoutIndex(int layoutIndex) {
			layoutIndex_ = layoutIndex;
		}

	protected:
		Component() : visible_(true),
			parent_(nullptr), focus_(false), layoutIndex_(0) {
		}
		
		// Takes care of all mouse events. And send it through to
		// all mouse listener callbacks.
		// Mouse events: SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN and SDL_MOUSEBUTTONUP.
		virtual void handleMouse(const SDL_Event& mouseEvent) {
			mouseListener_(this, mouseEvent);
		}

		// Takes care of all key events. And send it through to
		// all key listener callbacks.
		// Key events: SDL_TEXTINPUT, SDL_TEXTEDITING, SDL_KEYDOWN and SDL_KEYUP.
		virtual void handleKeyboard(const SDL_Event& keyEvent) {
			keyListener_(this, keyEvent);
		}

		// Fixes all child components sizes.
		// If there are no childs nothing happens.
		virtual void validate() {
		}

		// Is called in order to signal tha parent component that
		// the childrens sizes must be recalculated.
		void validateParent() {
			if (parent_ != nullptr) {
				parent_->validate();
			}
		}

	private:
		mw::TexturePtr background_;
		mw::Color backgroundColor_;
		Component* parent_;
		Point location_;
		Dimension dimension_;
		Dimension preferedDimension_;
		Dimension alignment_;
		int layoutIndex_;

		FocusListener focusListener_;
		KeyListener keyListener_;
		MouseListener mouseListener_;
		ActionListener actionListener_;
		PanelChangeListener panelChangeListener_;

		bool visible_;
		bool focus_;
	};	

} // Namespace gui.

#endif // GUI_COMPONENT_H
