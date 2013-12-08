#include <gui/guiwindow.h>
#include <gui/widget.h>
#include <gui/flowlayout.h>
#include <gui/panel.h>
#include <gui/component.h>

#include <cassert>

int main(int argc, char** argv) {
	gui::GuiWindow gui;
	gui::GuiWindow::startLoop();

	return 0;
}
