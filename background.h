#ifndef VIEW_H
#define VIEW_H

#include <memory>

namespace gui {

	class Background {
	public:
		// Draws the view.
		virtual void draw(int width, int height) = 0;
	};

	typedef std::shared_ptr<Background> BackgroundPtr;

} // Namespace gui.

#endif // VIEW_H
