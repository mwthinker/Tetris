#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "multiframe.h"
#include "textitem.h"
#include "textbutton.h"
#include "textbox.h"
#include "group.h"
#include "highscore.h"
#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>

#include <tuple>
#include <string>
#include <algorithm>

namespace gui {

	class GuiWindow : public mw::Window {
	public:
		GuiWindow();

		void test3();
		void test2();
		void test1();

	private:		

		bool isDrawGame() const;
		void setDrawGame(bool drawGame);		

		virtual void updateGame(Uint32 deltaTime) {
		}

		virtual void updateGameEvent(const SDL_Event& windowEvent) {
		}

		virtual void createCustomGame(int width, int height, int maxLevel) {
		}

		virtual void pauseGame() {
		}

		virtual void restartGame() {
		}

		void quit();

		ButtonPtr createTextButton(std::string text, int size, std::function<void()> onClick);
		TextBoxPtr createTextBox(int size);

		void initFrameMenu();
		void initPlayFrame();
		void initHighscoreFrame();
		void initCustomPlayFrame();
		void initOptionFrame();
		void initCreateClientFrame();

		void resize(int width, int height) override;

		// Override mw::Window
		void update(Uint32 deltaTime) override;

		// Override mw::Window
		void eventUpdate(const SDL_Event& windowEvent) override;
		
		int hDistance_;
		
		MultiFrame multiFrame_;
		bool fixSize_;

		int playFrameIndex_;
		int highscoreFrameIndex_;
		int customFrameIndex_;
		int optionFrameIndex_;

		// Attributes defined in void initCustomPlayFrame.
		TextBoxPtr customPlayWidth_, customPlayHeight_, customPlaymaxLevel_;
	};

} // Namespace gui.

#endif // GUIWINDOW_H
