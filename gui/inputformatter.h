#ifndef INPUTFORMATTER_H
#define INPUTFORMATTER_H

#include <sstream> // std::ostringstream
#include <cstring> // memmove

#include <memory>

namespace gui {

	class InputFormatter {
	public:
		enum Input {INPUT_MOVE_MARKER_LEFT,INPUT_MOVE_MARKER_RIGHT,INPUT_ERASE_LEFT,INPUT_ERASE_RIGHT}; //,INPUT_PASTE,INPUT_COPY};
		static int const MAX_SIZE = 100;		// Max number of input characters.

		InputFormatter(int maxLimit = 30) {
			clear();
			isWritable_ = true;
			maxLimit_ = maxLimit;
		}

		virtual ~InputFormatter() {
		}

		// Returns the current input.
		std::string getText() const {
			std::ostringstream stream;
			for (int i = 0; i < size_; ++i) {
				stream << text_[i];
			}
			return stream.str();
		}

		// Cleares the current input.
		void clear() {
			size_ = 0;
			marker_ = 0;
			for (int i = 0; i < MAX_SIZE; ++i) {
				text_[i] = ' ';
			}
		}

		// Updates the input.
		void update(char key) {
			if (isWritable_) {
				if (size_ < maxLimit_ && size_ < MAX_SIZE && isValidKey(key)) {
					memmove(text_+marker_+1,text_+marker_,size_ - marker_);
					text_[marker_] = key;
					++marker_;
					++size_;
				}
			}
		}

		// Updates the input.
		void update(InputFormatter::Input input) {
			if (isWritable_) {
				switch (input) {
				case INPUT_ERASE_LEFT:
					if (marker_ > 0) {
						removeChar(--marker_);
					}
					break;
				case INPUT_ERASE_RIGHT:
					if (marker_  < size_) {
						removeChar(marker_);
					}
					break;
				case INPUT_MOVE_MARKER_LEFT:
					if (marker_ > 0) {
						--marker_;
					}
					break;
				case INPUT_MOVE_MARKER_RIGHT:
					if (marker_ < size_) {
						++marker_;
					}
					break;
				}
			}
		}

		// Enables/disable the update functions.
		void setWritable(bool isWritable) {
			isWritable_ = isWritable;
		}

		// Returns true if the updatefunctions is active else false.
		bool isWritable() const {
			return isWritable_;
		}

		// Returns true if this InputTextfield contains input.
		bool hasInput() const {
			return size_ > 0;
		}

		int getMarkerPosition() const {
			return marker_;
		}

	protected:
		virtual bool isValidKey(char key) const {
			if (key == ' ') {
				return true;
			}
			if (key >= 'a' && key <= 'z') {
				return true;
			}
			if (key >= 'A' && key <= 'Z') {
				return true;
			}
			if (key >= '0' && key <= '9') {
				return true;
			}

			// Swedish keyboard.
			if (key == 'å' || key == 'ä' || key == 'ö' || key == 'Å' || key == 'Ä' || key == 'Ö') {
				return true;
			}
			return false;
		}

		char characterAt(int index) const {
			return text_[index];
		}

		int getSize() const {
			return size_;
		}

		void setSize(int size) {
			if (size < MAX_SIZE) {
				size_ = size;
			}
		}
	private:
		void removeChar(int index) {
			if (marker_ >= 0 && marker_ < size_) {
				memmove(text_+index,text_+index+1, size_ - index -1);
				--size_;
			}
		}

		int maxLimit_;			// Max number of input character allowed.
		bool isWritable_;		// New input is allowed.
		char text_[MAX_SIZE];	// Storage of text input.
		int size_;				// Size of string input.
		int marker_;			// Marker position.
	};

	typedef std::shared_ptr<InputFormatter> InputFormatterPtr;

} // Namespace gui.

#endif // INPUTFORMATTER_H
