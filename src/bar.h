#ifndef BAR_H
#define BAR_H

namespace gui {

	class Bar {
	public:
		enum Type {UP,DOWN,LEFT,RIGHT};

		Bar(Type type, int size) {
			type_ = type;
			size_ = size;
		}

		virtual ~Bar() {
		}

		virtual void draw(int width, int height) = 0;

		int getSize() const {
			return size_;
		}

		Type getType() const {
			return type_;
		}

	private:
		Type type_;
		int size_;
	};

} // Namespace gui.

#endif // BAR_H
