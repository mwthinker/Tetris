#ifndef GUI_TEXTFIELD_H
#define GUI_TEXTFIELD_H

#include "component.h"
#include "inputformatter.h"

#include <mw/text.h>
#include <mw/font.h>

#include <string>

namespace gui {

	class TextField : public Component {
	public:
		enum Alignment {
			LEFT,
			CENTER,
			RIGHT
		};

		TextField(const mw::FontPtr& font);

		TextField(std::string initialText , const mw::FontPtr& font);

		// Get the current text.
		std::string getText() const;

		// Set the current text.
		void setText(std::string text);

		// Set the textfield to be editable or not.
		void setEditable(bool editable);

		bool isEditable() const;

		virtual void draw(float deltaTime) override;

		Alignment getAlignment() const;
		void setAlignment(Alignment alignment);

	protected:
		void drawText(float deltaTime);

		void drawBorder();
		
		// Sets the color for the text.
		void setTextColor(const mw::Color& textColor);

	private:
		void init(std::string initialText, const mw::FontPtr& font);
		void handleKeyboard(const SDL_Event& keyEvent) override;

		bool editable_;
		mw::Text text_;
		float markerWidth_;
		bool markerChanged_;
		mw::FontPtr font_;

		mw::Color color_;
		Alignment alignment_;
		InputFormatter inputFormatter_;
		float markerDeltaTime_;
	};

} // Namespace gui.

#endif // GUI_TEXTFIELD_H
