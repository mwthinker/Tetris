#include <gui/frame.h>
#include <gui/flowlayout.h>
#include <gui/borderlayout.h>
#include <gui/panel.h>
#include <gui/component.h>
#include <gui/tbutton.h>

#include <cassert>

void testBorderLayout() {
	gui::Frame frame;
	frame.add(new gui::TButton, gui::BorderLayout::NORTH);
	frame.add(new gui::TButton, gui::BorderLayout::CENTER);
	frame.add(new gui::TButton, gui::BorderLayout::WEST);
	frame.add(new gui::TButton, gui::BorderLayout::EAST);
	frame.add(new gui::TButton, gui::BorderLayout::SOUTH);
	gui::Frame::startLoop();
}

void testFlowLayout() {
	gui::Frame frame;
	frame.setLayout(new gui::FlowLayout(gui::FlowLayout::LEFT));
	frame.add(new gui::TButton);
	frame.add(new gui::TButton);
	gui::Button* button = new gui::TButton();
	button->setPreferredSize(100, 100);
	frame.add(button);
	frame.add(new gui::TButton);
	frame.add(new gui::TButton);

	frame.addMouseListener([](gui::Component* c, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_MOUSEBUTTONDOWN:
				std::cout << "\n" << sdlEvent.button.x << " " << sdlEvent.button.y;
				break;
		}
	});

	gui::Frame::startLoop();
}

int main(int argc, char** argv) {
	testBorderLayout();
	testFlowLayout();

	return 0;
}
