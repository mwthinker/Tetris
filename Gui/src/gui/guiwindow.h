#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>
#include <mw/color.h>

#include <string>
#include <array>
#include <functional>
#include <queue>

namespace gui {

	class GuiWindow : public mw::Window {
	public:
		GuiWindow();

	private:
		// Override mw::Window
		void update(Uint32 deltaTime) override;

		// Override mw::Window
		void eventUpdate(const SDL_Event& windowEvent) override;

		void resize();		

		std::queue<SDL_Event> eventQueue_;

		int playFrameIndex_, networkFrameIndex_, highscoreFrameIndex_,
			customFrameIndex_, optionFrameIndex_, newHighscoreFrameIndex_,
			createClientFrameIndex_, createServerFrameIndex_, loobyClientFrameIndex_,
			loobyServerFrameIndex_, waitToConnectFrameIndex_, networkPlayFrameIndex_,
			aiFrameIndex_;
	};

} // Namespace gui.

#endif // GUIWINDOW_H
