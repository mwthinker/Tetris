#ifndef GUI_INPUTFORMATTER_H
#define GUI_INPUTFORMATTER_H

#include <sstream> // std::ostringstream
#include <cstring> // std::memmove

namespace gui {

	class InputFormatter {
	public:
		static int const MAX_SIZE = 100; // Max number of input characters.

		enum Input {
			INPUT_MOVE_MARKER_HOME,
			INPUT_MOVE_MARKER_END,
			INPUT_MOVE_MARKER_LEFT,
			INPUT_MOVE_MARKER_RIGHT,
			INPUT_ERASE_LEFT,
			INPUT_ERASE_RIGHT
		};

		InputFormatter(int maxLimit = 30) {
			size_ = 0;
			marker_ = 0;
			maxLimit_ = maxLimit;
		}

		// Returns the current input.
		// The return value is a utf8 string.
		std::string getText() const {
			std::ostringstream stream;
			for (int i = 0; i < size_; ++i) {
				stream << text_[i];
			}
			return stream.str();
		}

		// Clears the current input.
		void clear() {
			size_ = 0;
			marker_ = 0;
		}

		// Takes a utf8 character as input.
		// The whole c-string assumes to represent a utf8 character.
		void update(const char* text) {
			int size = std::strlen(text);

			if (size_ + size <= maxLimit_ && size_ + size <= MAX_SIZE) {
				memmove(text_ + marker_ + size, text_ + marker_, size_ - marker_);
				memmove(textUtf8_ + marker_ + size, textUtf8_ + marker_, size_ - marker_);
				for (int i = 0; i < size; ++i) {
					text_[marker_] = text[i];
					textUtf8_[marker_] = size;
					++marker_;
					++size_;
				}
			}
		}

		// Updates the input.
		void update(InputFormatter::Input input) {
				switch (input) {
					case INPUT_ERASE_LEFT:
						removeChar(true);
						break;
					case INPUT_ERASE_RIGHT:
						removeChar(false);
						break;
					case INPUT_MOVE_MARKER_LEFT:
						if (marker_ > 0) {
							marker_ += -textUtf8_[marker_ - 1];
						}
						break;
					case INPUT_MOVE_MARKER_RIGHT:
						if (marker_ < size_) {
							marker_ += textUtf8_[marker_];
						}
						break;
					case INPUT_MOVE_MARKER_HOME:
						marker_ = 0;
						break;
					case INPUT_MOVE_MARKER_END:
						marker_ = size_;
						break;
				}			
		}

		// Returns the size of the current utf8 string.
		int getSize() const {
			return size_;
		}

		// Get the position for the marker. The position
		// is for the character in the utf8 string.
		int getMarkerPosition() const {
			return marker_;
		}

	private:
		// Remove one utf8 encoded character to the left or to the right of the marker.
		// May remove several character due to the encoding.
		void removeChar(bool leftMarker) {
			if (leftMarker && marker_ > 0) {
				int dst = marker_ - textUtf8_[marker_ - 1];
				int src = marker_;
				int size = size_ - marker_;
				memmove(text_ + dst, text_ + src, size);
				size_ -= textUtf8_[marker_ - 1];
				marker_ -= textUtf8_[marker_ - 1];
				memmove(textUtf8_ + dst, textUtf8_ + src, size);
			} else if (!leftMarker && marker_ < size_) {
				int dst = marker_;
				int src = marker_ + textUtf8_[marker_];
				int size = size_ - (marker_ + textUtf8_[marker_]);
				memmove(text_ + dst, text_ + src, size);
				size_ -= textUtf8_[marker_];
				memmove(textUtf8_ + dst, textUtf8_ + src, size);
			}
		}

		int maxLimit_;				// Max number of input character allowed.
		char text_[MAX_SIZE];		// Storage of text input.
		char textUtf8_[MAX_SIZE];	// Correspond to number of bytes corresponding utf8 takes.
		int size_;					// Size of string input.
		int marker_;				// Marker position.
	};

} // Namespace gui.

#endif // GUI_INPUTFORMATTER_H
