#ifndef FRAME_H
#define FRAME_H

#include "bar.h"
#include "guiitem.h"
#include "color.h"
#include "background.h"

#include <mw/window.h>

#include <functional>
#include <cmath>
#include <vector>
#include <SDL_opengl.h>
#include <memory>

namespace gui {

	// Responsible of delegating windows events and drawing the added elements.
	// The elements are of three different classes, i.e. BarPtr, GuiItemPtr and BackgroundPtr.
	class Frame {
	public:
		// Creates a empty frame.
		Frame();

		// Sets all guiitems to be unfocused.
		void unfocusAll();

		// Sets the current background.
		void setBackground(BackgroundPtr background);

		// Add a bar.
		void addBar(BarPtr bar);

		// Add a gui item at postion (x,y). By default origo is at the bottom left of the window.
		// Y-axis is directed up and X-axis is directed right. If invX and/or invY is true then the 
		// respectiv axis direction is inverted.
		void add(GuiItemPtr guiItem, int x, int y, bool invX = false, bool invY = false);

		// Updates all guitems in order for them to repsond to a window event.
		void eventUpdate(const SDL_Event& windowEvent);
		
		// Draws all elements in frame.
		void draw();

	private:
		class GuiProperties {
		public:
			GuiProperties();

			GuiProperties(int x, int y, bool invX, bool invY);

			int x_, y_;  // Global position.
			bool invX_, invY_;  // Invert the x and y axis if true.	
		};

		static void windowSize(int& width, int& height);

		static void mousePosition(int& x, int& y);

		typedef std::pair<GuiItemPtr,GuiProperties> Item_;
		std::vector<Item_> items_;

		typedef std::pair<BarPtr,GuiProperties> BarItem_;
		std::vector<BarItem_> barItems_;

		BackgroundPtr background_;
	};

	typedef std::shared_ptr<Frame> FramePtr;

} // Namespace gui.

#endif // FRAME_H
