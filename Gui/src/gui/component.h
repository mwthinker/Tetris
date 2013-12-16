#ifndef COMPONENT_H
#define COMPONENT_H

#include "dimension.h"

#include <mw/signal.h>
#include <mw/color.h>
#include <mw/texture.h>

#include <sdl.h>

namespace gui {

	class Component;

	using FocusListener = mw::Signal<Component*, const SDL_Event&>;
	using KeyListener = FocusListener;
	using MouseListener = KeyListener;
	using ActionListener = mw::Signal<Component*>;

	enum Orientation {
		LT
	};

	class Component {
	public:
		friend class Panel;

		virtual ~Component() {
		}

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
			setNotValid();
		}

		void setPreferredSize(const Dimension& dimension) {
			preferedDimension_ = dimension;
			setNotValid();
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

		void setSize(const Dimension& dimension) {
			dimension_ = dimension;
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

		void setToValid() {
			valid_ = true;
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

		void setFocus(bool focus) {
			focus_ = focus;
		}

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
		// Should be deriverd and should then draw the
		// component in the size defined by getSize().
		virtual void draw(float deltaTime) {
			mw::TexturePtr texture = getBackground();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			if (texture) {
				glEnable(GL_TEXTURE_2D);
				getBackground()->bind();
			}			

			// Draw panel background.
			backgroundColor_.glColor4d();

			Dimension dim = getSize();
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0.f, 0.f);
			glTexCoord2f(1, 0);
			glVertex2f(dim.width_*1.f, 0.f);
			glTexCoord2f(1, 1);
			glVertex2f(dim.width_*1.f, dim.height_*1.0f);
			glTexCoord2f(0, 1);
			glVertex2f(0.f, dim.height_*1.f);
			glEnd();

			if (texture) {
				glDisable(GL_TEXTURE_2D);
			}

			glDisable(GL_BLEND);
		}

		// Do action.
		void doAction() {
			actionListener_(this);
		}

		// Only called when the mouse leave the component.
		virtual void mouseMotionLeave() {
		}

		// Only called when the mouse down was inside the component.
		// And the up event was outside.
		virtual void mouseOutsideUp() {
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
		Component() : valid_(false), visible_(true), parent_(nullptr), focus_(false), layoutIndex_(0) {
		}

		virtual void handleMouse(const SDL_Event& mouseEvent) {
			mouseListener_(this, mouseEvent);
		}

		virtual void handleKeyboard(const SDL_Event& keyEvent) {
			keyListener_(this, keyEvent);
		}

	private:
		void setNotValid() {
			// Set this and all parents invalid.
			Component* parent = this;
			while (parent != nullptr) {
				parent->valid_ = false;
				parent = parent->parent_;
			}
		}

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

		bool visible_;
		bool valid_;
		bool focus_;
	};	

} // Namespace gui.

#endif // COMPONENT_H
