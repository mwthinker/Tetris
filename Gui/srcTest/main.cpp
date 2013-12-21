#include <gui/frame.h>
#include <gui/flowlayout.h>
#include <gui/borderlayout.h>
#include <gui/panel.h>
#include <gui/component.h>
#include <gui/label.h>
#include <gui/button.h>
#include <gui/textfield.h>
#include <gui/checkbox.h>
#include <gui/boxlayout.h>
#include <gui/gridlayout.h>

#include <cassert>

void testBorderLayout(gui::Frame& frame) {
	frame.add(new gui::Button, gui::BorderLayout::NORTH);
	frame.add(new gui::Button, gui::BorderLayout::CENTER);
	frame.add(new gui::Button, gui::BorderLayout::WEST);
	frame.add(new gui::Button, gui::BorderLayout::EAST);
	frame.add(new gui::Button, gui::BorderLayout::SOUTH);
}

void testFlowLayout(gui::Frame& frame) {
	mw::FontPtr font(new mw::Font("Ubuntu-B.ttf", 16));
	frame.setLayout(new gui::FlowLayout(gui::FlowLayout::RIGHT));
	frame.add(new gui::Button("Button 1", font));
	frame.add(new gui::Button("Button 2", font));
	gui::Button* button = new gui::Button("Button 3", font);
	button->setVerticalAlignment(gui::Button::TOP);
	frame.add(button);
	frame.add(new gui::Button("Button 4", font));
	frame.add(new gui::CheckBox("CheckBox 5", font));
	frame.add(new gui::TextField("Text:", font));
	frame.add(new gui::Label("JAjajaj", font));

	gui::Panel* panel = new gui::Panel();
	panel->setBackgroundColor(mw::Color(0, 1, 0));
	frame.add(panel);

	frame.addMouseListener([](gui::Component* c, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_MOUSEBUTTONDOWN:
				std::cout << "\n" << sdlEvent.button.x << " " << sdlEvent.button.y;
				break;
		}
	});
}

// Objects can be drawn outside the panel. Fix todo!

int main(int argc, char** argv) {
	//gui::Frame frameBorder;
	//frameBorder.setDefaultClosing(true);
	//testBorderLayout(frameBorder);
	gui::Frame frameFlow;
	frameFlow.setDefaultClosing(true);
	testFlowLayout(frameFlow);
	
	SDL_StartTextInput();
	gui::Frame::startLoop();

	return 0;
}
