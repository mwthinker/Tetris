#ifndef GUI_DIMENSION_H
#define GUI_DIMENSION_H

namespace gui {

	class Point {
	public:
		Point() : x_(0), y_(0) {
		}

		Point(float x, float y) : x_(x), y_(y) {
		}

		float x_;
		float y_;
	};

	class Dimension {
	public:
		Dimension() : width_(0), height_(0) {
		}

		Dimension(float width, float height) : width_(width), height_(height) {
		}

		Dimension operator+(const Dimension& dim) {
			return Dimension(dim.width_ + width_, dim.height_ + height_);
		}

		float width_;
		float height_;
	};

} // Namespace gui.

#endif // GUI_DIMENSION_H
