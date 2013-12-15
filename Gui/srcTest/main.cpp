#include <gui/frame.h>
#include <gui/flowlayout.h>
#include <gui/panel.h>
#include <gui/component.h>

#include <cassert>

int main(int argc, char** argv) {
	gui::Frame gui;
	gui::Frame::startLoop();

	return 0;
}
