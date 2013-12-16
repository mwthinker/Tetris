#ifndef LABEL_H
#define LABEL_H

#include "component.h"

namespace gui {

	class Label : public Component {
	public:
		enum Alignment {
			LEFT,
			CENTER,
			RIGHT
		};

		Label(std::string strLabel, Alignment alignment) {
		}

		std::string getText();
		void setText(std::string strLabel);

		Alignment getAlignment();
		void setAlignment(Alignment alignment);
	};

} // Namespace gui.

#endif // LABEL_H
