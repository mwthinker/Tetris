#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H

#include "button.h"

namespace gui {

	class CheckBox : public Button {
	public:
		CheckBox(int characterSize);
		CheckBox(std::string text, const mw::FontPtr& font);
		CheckBox(mw::Text text);

		virtual void draw(Uint32 deltaTime) override;

		bool isSelected() const {
			return selected_;
		}

		void setSelected(bool select) {
			selected_ = select;
		}

	private:
		float boxSize_;
		bool selected_;
	};

} // Namespace gui.

#endif // GUI_CHECKBOX_H
