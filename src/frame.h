#ifndef FRAME_H
#define FRAME_H

#include "guitypedefs.h"

#include <mw/color.h>
#include <mw/window.h>

#include <SDL.h>

#include <functional>
#include <vector>

namespace gui {

	// Responsible of delegating windows events and drawing the added elements.
	// The elements are of three different classes, i.e. BarPtr, GuiItemPtr and BackgroundPtr.
	class Frame {
	public:
		// Creates a empty frame.
		Frame(SDL_Window* window);

		// Sets all guiitems to be unfocused.
		void unfocusAll();

		// Sets the current background.
		void setBackground(BackgroundPtr background);

		// Gets the current background.
		BackgroundPtr getBackground() const;

		// Add a bar.
		void addBar(BarPtr bar);

		// Add a gui item at postion (x,y). By default origo is at the corner decided by (invX,invY) of the window.
		// Y-axis is directed up and X-axis is directed right. If invX and/or invY is true then the 
		// respectiv axis direction is inverted. (invX,invY)= (false,false) the origo is placed in the bottom left.
		void add(GuiItemPtr guiItem, int x, int y, bool invX = false, bool invY = false);

		// Updates all guitems in order for them to repsond to a window event.
		void eventUpdate(const SDL_Event& windowEvent);
		
		// Draws all elements in frame.
		void draw();

		void windowSize(int& width, int& height);
		
		SDL_Window* window_;

	private:
		class GuiProperties {
		public:
			GuiProperties();

			GuiProperties(int x, int y, bool invX, bool invY);

			int x_, y_;  // Global position.
			bool invX_, invY_;  // Invert the x and y axis if true.	
		};

		void mousePosition(int& x, int& y);

		typedef std::pair<GuiItemPtr,GuiProperties> Item_;
		std::vector<Item_> items_;

		typedef std::pair<BarPtr,GuiProperties> BarItem_;
		std::vector<BarItem_> barItems_;

		BackgroundPtr background_;
	};	

} // Namespace gui.

#endif // FRAME_H
