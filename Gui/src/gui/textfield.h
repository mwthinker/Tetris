#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include "component.h"

namespace gui {

	class TextField : public Component {
	public:
		TextField(std::string strInitialText);

		// Get the current text on this TextField instance.
		std::string getText() const;

		// Set the display text on this TextField instance.
		void setText(std::string strText);

		// Set this TextField to editable (read/write) or non-editable (read-only).
		void setEditable(bool editable);
	};

} // Namespace gui.

#endif // TEXTFIELD_H
