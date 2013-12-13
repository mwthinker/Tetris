#ifndef BUTTON_H
#define BUTTON_H

#include "component.h"

#include <mw/signal.h>
#include <mw/text.h>

#include <functional>
#include <string>

namespace gui {

	class Button : public Component {
	public:
		Button();

		// Get the label of this Button instance.
		std::string getLabel() const;

		// Set the label of this Button instance.
		void setLabel(std::string buttonLabel);

		void setFont(const mw::FontPtr& font);

		mw::FontPtr getFont(const mw::FontPtr& font) const;

		virtual void draw(float deltaTime) override;

	private:
		mw::Text text_;
	};

} // Namespace gui.

#endif // BUTTON_H
