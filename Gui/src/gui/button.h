#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "component.h"

#include <mw/signal.h>
#include <mw/text.h>

#include <functional>
#include <string>

namespace gui {

	class Button : public Component {
	public:
		enum HorizontalAlignment {
			LEFT,
			HCENTER,
			RIGHT
		};

		enum VerticalAlignment {
			TOP,
			VCENTER,
			BOTTOM
		};

		Button();
		Button(std::string text, const mw::FontPtr& font);
		Button(mw::Text text);

		virtual ~Button() {
		}

		virtual void draw(Uint32 deltaTime) override;

		// Get the label of the Button instance.
		std::string getLabel() const;

		// Sets the label of the Button instance.
		void setLabel(std::string buttonLabel);

		// Sets the font used by the label.
		void setFont(const mw::FontPtr& font);

		// Returns true when the mouse is inside the button. Else 
		// it returns false.
		bool isMouseInside() const;

		// Returns true if the button was pushed. Else 
		// it returns false.
		bool isPushed() const;

		// Returns true if the mouse left button is hold down when the 
		// mouse hovers the button. Else it returns false.
		bool isMouseDown() const;
		
		void mouseMotionLeave() override;

		void mouseOutsideUp() override;
		
		// Sets the vertical alignment for the text label.
		void setVerticalAlignment(VerticalAlignment alignment);
		
		// Sets the horizontal alignment for the text label.
		void setHorizontalAlignment(HorizontalAlignment alignment);

		inline const mw::Color& getTextColor() const {
			return textColor_;
		}

		// Sets the color for the text label.
		void setTextColor(const mw::Color& textColor);

		virtual void sizeToFitText();

		void setAutoSizeToFitText(bool autoFit);

		bool isAutoSizeToFitText() const;

		inline const mw::Color& getHoverColor() const {
			return hoverColor_;
		}

		void setHoverColor(const mw::Color& color) {
			hoverColor_ = color;
		}

		inline const mw::Color& getFocusColor() const {
			return focusColor_;
		}

		void setFocusColor(const mw::Color& color) {
			focusColor_ = color;
		}

		inline const mw::Color& getPushColor() const {
			return pushColor_;
		}

		void setPushColor(const mw::Color& color) {
			pushColor_ = color;
		}

	protected:
		// Is drawn if this butten has focus while the mouse 
		// button was is oustside this button.
		virtual void drawOnFocus();
		// Is drawn if the mouse is hovering and 
		// the mouse is not pushed down.
		virtual void drawOnMouseHover();
		// Is drawn if the button was pushed.
		virtual void drawOnPush();
		// Draws the label.
		virtual void drawLabel();

		mw::Text text_;
		mw::Text toWide_;

		mw::Color textColor_;
		mw::Color hoverColor_;
		mw::Color focusColor_;
		mw::Color pushColor_;

		VerticalAlignment vTextAlignment_;
		HorizontalAlignment hTextAlignment_;

		bool autoFit_;

	private:
		void init();

		void handleMouse(const SDL_Event&) override;
		void handleKeyboard(const SDL_Event&) override;
		
		bool pushed_;
		bool mouseDown_;
		bool mouseInside_;
	};

} // Namespace gui.

#endif // GUI_BUTTON_H
