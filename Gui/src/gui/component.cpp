#include "component.h"

namespace gui {

	void Component::setPreferredSize(float width, float height) {
		preferedDimension_ = Dimension(width, height);
		validateParent();
	}

	void Component::setPreferredSize(const Dimension& dimension) {
		preferedDimension_ = dimension;
		validateParent();
	}

	void Component::setVisible(bool visible) {
		if (visible_ != visible) {
			visible_ = visible;
			validateParent();
		}
	}

	mw::signals::Connection Component::addKeyListener(const KeyListener::Callback& callback) {
		return keyListener_.connect(callback);
	}

	mw::signals::Connection Component::addMouseListener(const MouseListener::Callback& callback) {
		return mouseListener_.connect(callback);
	}

	mw::signals::Connection Component::addFocusListener(const FocusListener::Callback& callback) {
		return focusListener_.connect(callback);
	}

	mw::signals::Connection Component::addActionListener(const ActionListener::Callback& callback) {
		return actionListener_.connect(callback);
	}

	mw::signals::Connection Component::addPanelChangeListener(const PanelChangeListener::Callback& callback) {
		return panelChangeListener_.connect(callback);
	}

	void Component::setGrabFocus(bool grabFocus) {
		if (grabFocus_ != grabFocus) {
			// State is changed.
			grabFocus_ = grabFocus;

			if (grabFocus) {
				setFocus(true);
				if (parent_ != nullptr) {
					// This component is one more.
					++parent_->nbrChildGrabFocus_;
				}
			} else {
				// Have children that wants focus?
				if (nbrChildGrabFocus_ > 1) {
					// Has a parent?
					if (parent_ != nullptr) {
						// One component less.
						--parent_->nbrChildGrabFocus_;
					}
				}
			}
		}
	}

	bool Component::isGrabFocus() const {
		return grabFocus_;
	}

	void Component::setFocus(bool focus) {
		// Change?
		if (focus_ != focus) {
			if (focus || grabFocus_ || (!focus && nbrChildGrabFocus_ < 1)) {
				focus_ = focus;
				focusListener_(this);
			}
		}
	}

	bool Component::hasFocus() const {
		return focus_;
	}

	void Component::draw(Uint32 deltaTime) {
		// Draw panel background.
		backgroundColor_.glColor4d();
		glPushMatrix();
		Dimension dim = getSize();
		glScaled(dim.width_, dim.height_, 1);
		glTranslated(0.5, 0.5, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(-0.5f, 0.5f);
		glEnd();
		glDisable(GL_BLEND);

		background_.draw();
		glPopMatrix();
		drawBorder();
	}

	void Component::doAction() {
		actionListener_(this);
	}

	void Component::panelChanged(bool active) {
		panelChangeListener_(this, active);
	}

	int Component::getLayoutIndex() const {
		return layoutIndex_;
	}
		
	void Component::setLayoutIndex(int layoutIndex) {
		layoutIndex_ = layoutIndex;
	}

	Component::Component() : parent_(nullptr), layoutIndex_(0), visible_(true),
		focus_(false), grabFocus_(false), isAdded_(false) {
		borderColor_ = mw::Color(0, 0, 0);
		nbrChildGrabFocus_ = 0;
	}

	void Component::handleMouse(const SDL_Event& mouseEvent) {
		mouseListener_(this, mouseEvent);
	}

	void Component::handleKeyboard(const SDL_Event& keyEvent) {
		keyListener_(this, keyEvent);
	}

	void Component::validateParent() {
		if (parent_ != nullptr) {
			parent_->validate();
		}
	}

	void Component::drawBorder() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		borderColor_.glColor4d();
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 0);
		glVertex2d(dimension_.width_, 0);
		glVertex2d(dimension_.width_, dimension_.height_);
		glVertex2d(0, dimension_.height_);
		glEnd();
		glDisable(GL_BLEND);
	}

} // Namespace gui.
