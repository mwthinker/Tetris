#ifndef INPUTNUMBERFORMATTER_H
#define INPUTNUMBERFORMATTER_H

#include "inputformatter.h"

namespace gui {

	class InputNumberFormatter : public InputFormatter {
	public:
		InputNumberFormatter(int maxLimit) : InputFormatter(maxLimit) {
		}

	private:
		bool isValidKey(char key) const override {
			if (key >= '0' && key <= '9') {
				return true;
			}
			return false;
		}
	};

} // Namespace gui.

#endif // INPUTNUMBERFORMATTER_H
