#ifndef VIEW_H
#define VIEW_H

namespace gui {

	class Background {
	public:
		// Draws the view.
		virtual void draw(int width, int height) = 0;
	};	

} // Namespace gui.

#endif // VIEW_H
