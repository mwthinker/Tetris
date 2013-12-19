#include <gui/frame.h>
#include <gui/flowlayout.h>
#include <gui/borderlayout.h>
#include <gui/panel.h>
#include <gui/component.h>
#include <gui/tbutton.h>
#include <gui/label.h>
#include <gui/textfield.h>
#include <gui/checkbox.h>

#include <cassert>

gui::TButton* createButton(std::string text) {
	mw::Color textColor(1, 1, 1);
	mw::Color textButtonColor(1, .1, .1);
	mw::Color focusColo(.8, .1, 0, .3);
	mw::Color onHoverColor(.6, .1, .1);
	mw::Color notHoverColor(.4, .0, .0, .0);
	mw::Color pushedColor(.8, .0, 0, .7);
	return nullptr;
	//return new TButton(text, size, fontDefault, textButtonColor_, focusColor_, onHoverColor_, notHoverColor_, pushedColor_);			
}

void testBorderLayout(gui::Frame& frame) {
	frame.add(new gui::TButton, gui::BorderLayout::NORTH);
	frame.add(new gui::TButton, gui::BorderLayout::CENTER);
	frame.add(new gui::TButton, gui::BorderLayout::WEST);
	frame.add(new gui::TButton, gui::BorderLayout::EAST);
	frame.add(new gui::TButton, gui::BorderLayout::SOUTH);
}

void testFlowLayout(gui::Frame& frame) {
	mw::FontPtr font(new mw::Font("Ubuntu-B.ttf", 16));
	frame.setLayout(new gui::FlowLayout(gui::FlowLayout::RIGHT));
	frame.add(new gui::TButton("Button 1", font));
	frame.add(new gui::TButton("Button 2", font));
	gui::Button* button = new gui::TButton("Button 3", font);
	button->setVerticalAlignment(gui::Button::TOP);
	frame.add(button);
	frame.add(new gui::TButton("Button 4", font));
	frame.add(new gui::CheckBox("CheckBox 5", font));
	frame.add(new gui::TextField("Text:", font));
	frame.add(new gui::Label("JAjajaj", font));

	frame.addMouseListener([](gui::Component* c, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_MOUSEBUTTONDOWN:
				std::cout << "\n" << sdlEvent.button.x << " " << sdlEvent.button.y;
				break;
		}
	});
}

int main(int argc, char** argv) {
	gui::Frame frameBorder;
	frameBorder.setDefaultClosing(true);
	testBorderLayout(frameBorder);
	//gui::Frame frameFlow;
	//frameFlow.setDefaultClosing(true);
	//testFlowLayout(frameFlow);
	
	SDL_StartTextInput();
	gui::Frame::startLoop();

	return 0;
}
