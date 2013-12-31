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
			nbrOfCharacters_ = 0;
		}

		int getMaxLimit() const {
			return maxLimit_;
		}

		void setMaxLimit(int maxLimit) {
			if (maxLimit < MAX_SIZE) {
				maxLimit_ = maxLimit;
			}
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
			nbrOfCharacters_ = 0;
		}

		// Takes a c-string which must be encoded in utf8 in order
		// to work correctly.
		void update(const char* text) {
			int size = std::strlen(text);
			int length = 1;
			for (int i = 0; i < size; i += length) {
				unsigned char token = text[i];
				if (token < UTF8_BYTE_2) {
					length = 1;
				} else if (token < UTF8_BYTE_3) {
					length = 2;
				} else if (token < UTF8_BYTE_4) {
					length = 3;
				} else if (token < UTF8_BYTE_5) {
					length = 4;
				} else if (token < UTF8_BYTE_6) {
					length = 5;
				} else {
					length = 6; // Assumes.
				}
				if (nbrOfCharacters_ + length > maxLimit_) {
					break;
				}
				addUtf8(text + i, length);
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

		int getNbrOfCharacters() const {
			return nbrOfCharacters_;
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
				nbrOfCharacters_ -= textUtf8_[marker_ - 1];
				int dst = marker_ - textUtf8_[marker_ - 1];
				int src = marker_;
				int size = size_ - marker_;
				memmove(text_ + dst, text_ + src, size);
				size_ -= textUtf8_[marker_ - 1];
				marker_ -= textUtf8_[marker_ - 1];
				memmove(textUtf8_ + dst, textUtf8_ + src, size);
			} else if (!leftMarker && marker_ < size_) {
				nbrOfCharacters_ -= textUtf8_[marker_];
				int dst = marker_;
				int src = marker_ + textUtf8_[marker_];
				int size = size_ - (marker_ + textUtf8_[marker_]);
				memmove(text_ + dst, text_ + src, size);
				size_ -= textUtf8_[marker_];
				memmove(textUtf8_ + dst, textUtf8_ + src, size);
			}
		}

		void addUtf8(const char* utf8Token, int length) {
			if (size_ + length <= maxLimit_ && size_ + length <= MAX_SIZE) {
				memmove(text_ + marker_ + length, text_ + marker_, size_ - marker_);
				memmove(textUtf8_ + marker_ + length, textUtf8_ + marker_, size_ - marker_);
				for (int i = 0; i < length; ++i) {
					text_[marker_] = utf8Token[i];
					textUtf8_[marker_] = length;
					++marker_;
					++size_;
				}
				nbrOfCharacters_ += length;
			}
		}

		// First code point. Source: https://en.wikipedia.org/wiki/UTF-8.
		// Only the value of the first byte is of interest in order to see how many bytes
		// each utf8 character contains.
		unsigned const char UTF8_BYTE_1 = 0;
		unsigned const char UTF8_BYTE_2 = 192;
		unsigned const char UTF8_BYTE_3 = 224;
		unsigned const char UTF8_BYTE_4 = 240;
		unsigned const char UTF8_BYTE_5 = 248;
		unsigned const char UTF8_BYTE_6 = 252;

		int maxLimit_;				// Max number of input character allowed.
		char text_[MAX_SIZE];		// Storage of text input.
		char textUtf8_[MAX_SIZE];	// Correspond to number of bytes corresponding utf8 takes.
		int size_;					// Size of the string.
		int marker_;				// Marker position.
		int nbrOfCharacters_;		// The number of readable (for a human) characters.
	};

} // Namespace gui.

#endif // GUI_INPUTFORMATTER_H
