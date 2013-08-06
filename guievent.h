#ifndef GUIEVENT_H
#define GUIEVENT_H

#include <SDL.h>

namespace gui {

	class GuiItem;

	class GuiEvent {
	public:
		GuiEvent(GuiItem& guiItem) : guiItem_(guiItem) {
		}

		GuiItem& guiItem_;
	};

	class FrameChanged : public GuiEvent {
	public:
		FrameChanged(GuiItem& guiItem, int lastFrame, int newFrame) : GuiEvent(guiItem), lastFrame_(lastFrame), newFrame_(newFrame) {
		}

		int lastFrame_;
		int newFrame_;
	};

	class MouseDown : public GuiEvent {
	public:
		enum Button {
			LEFT,
			MIDDLE,
			RIGHT
		};

		MouseDown(GuiItem& guiItem, Button button, double x, double y) : GuiEvent(guiItem), x_(x), y_(y), button_(button) {
		}

		double x_, y_;
		Button button_;
	};

	class MouseUp : public GuiEvent {
	public:
		enum Button {
			LEFT,
			MIDDLE,
			RIGHT
		};

		MouseUp(GuiItem& guiItem, Button button, double x, double y) : GuiEvent(guiItem), x_(x), y_(y), button_(button) {
		}

		double x_, y_;
		Button button_;
	};

	class MouseMotion : public GuiEvent {
	public:
		MouseMotion(GuiItem& guiItem, double x, double y) : GuiEvent(guiItem), x_(x), y_(y) {
		}

		double x_, y_;
	};

	class WindowsActive : public GuiEvent {
	public:
		WindowsActive(GuiItem& guiItem, bool active) : GuiEvent(guiItem), active_(active) {
		}

		bool active_;
	};

	class KeyUp : public GuiEvent {
	public:
		KeyUp(GuiItem& guiItem, SDLKey key) : GuiEvent(guiItem), key_(key) {
		}

		SDLKey key_;
	};

	class KeyDown : public GuiEvent {
	public:
		KeyDown(GuiItem& guiItem, SDLKey key) : GuiEvent(guiItem), key_(key) {
		}

		SDLKey key_;
	};

} // Namespace gui.

#endif // GUIEVENT_H
