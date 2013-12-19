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

		virtual void draw(float deltaTime) override;

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

		// Sets the color for the text label.
		void setTextColor(const mw::Color& textColor);

		virtual void preferedSizeFitText();

	protected:
		void drawLabel();

		mw::Text text_;
		mw::Color textColor_;
		VerticalAlignment vTextAlignment_;
		HorizontalAlignment hTextAlignment_;

	private:
		inline void init() {
			setPreferredSize(50, 50);
			std::bind(&Button::handleMouse, this, this, std::placeholders::_1);
			std::bind(&Button::handleKeyboard, this, this, std::placeholders::_1);
			pushed_ = false;
			mouseDown_ = false;
			mouseInside_ = false;
			vTextAlignment_ = VerticalAlignment::VCENTER;
			hTextAlignment_ = HorizontalAlignment::HCENTER;
			textColor_ = mw::Color(0, 0, 0);
			setBackgroundColor(mw::Color(0.9, 0.9, 0.9));

			Button::preferedSizeFitText();
		}

		void handleMouse(const SDL_Event&);
		void handleKeyboard(const SDL_Event&);
		
		bool pushed_;
		bool mouseDown_;
		bool mouseInside_;
	};

} // Namespace gui.

#endif // GUI_BUTTON_H
