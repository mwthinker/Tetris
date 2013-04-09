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

	class Frame {
	public:
		Frame();

		void unfocusAll();
		void setBackground(BackgroundPtr background);

		void addBar(BarPtr bar);
		void add(GuiItemPtr guiItem, int x, int y, bool invX = false, bool invY = false);

		void eventUpdate(const SDL_Event& windowEvent);
		void draw();

	private:
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
