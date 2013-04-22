#include "multiframe.h"

#include "guiitem.h"
#include "bar.h"
#include "frame.h"

#include <mw/color.h>

#include <vector>
#include <SDL_opengl.h>

using namespace gui;

MultiFrame::MultiFrame() {
	currentFrame_ = 0;
	addFrameBack();
}

// Get the panel corrensponding to the frame with index (index).
FramePtr MultiFrame::operator[](int index) {
	return frames_[index];
}

void MultiFrame::unfocus(int indexFrame) {
	frames_[indexFrame]->unfocusAll();
}

// Creates a new frame at the back.
int MultiFrame::addFrameBack() {
	frames_.push_back(std::make_shared<Frame>());
	return frames_.size() - 1;
}

void MultiFrame::add(GuiItemPtr guiItem, int x, int y, bool invX, bool invY) {
	frames_[currentFrame_]->add(guiItem, x, y, invX, invY);
}

// Add item (item) to frame with index (frameIndex).
void MultiFrame::add(GuiItemPtr guiItem, int x, int y, bool invX, bool invY, int frameIndex) {
	frames_[frameIndex]->add(guiItem, x, y, invX, invY);
}

// Is called in order to update all items in the active frame.
void MultiFrame::eventUpdate(const SDL_Event& windowEvent) {
	frames_[currentFrame_]->eventUpdate(windowEvent);
}

void MultiFrame::setBackground(BackgroundPtr background) {
	frames_[currentFrame_]->setBackground(background);
}

void MultiFrame::setBackground(BackgroundPtr background, int frameIndex) {
	frames_[frameIndex]->setBackground(background);
}

void MultiFrame::addBar(BarPtr bar) {
	frames_[currentFrame_]->addBar(bar);
}

void MultiFrame::addBar(BarPtr bar, int frameIndex) {
	frames_[frameIndex]->addBar(bar);
}

void MultiFrame::draw(double timeStep) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int windowsWidth, windowsHeight;
	GuiItem::windowSize(windowsWidth,windowsHeight);
	glViewport(0,0,windowsWidth,windowsHeight);		
	glOrtho(0,windowsWidth,0,windowsHeight,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();		
	glLoadIdentity();
	frames_[currentFrame_]->draw();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

int MultiFrame::getNbrOfFrames() const {
	return frames_.size();
}

int MultiFrame::getCurrentFrameIndex() const {
	return currentFrame_;
}

void MultiFrame::setCurrentFrame(int index) {
	SDL_Event sdlEvent;
	sdlEvent.user.type = SDL_USEREVENT;
	sdlEvent.user.code = GUI_CHANGE_FROM_FRAME_CODE;
	sdlEvent.user.data1 = 0;
	sdlEvent.user.data2 = 0;
	
	// Update all current items that their frame is being inactivated.
	eventUpdate(sdlEvent);

	currentFrame_ = index;
	
	// Update the new current items that their frame is being activated.
	sdlEvent.user.code = GUI_CHANGE_TO_FRAME_CODE;
	eventUpdate(sdlEvent);
}

FramePtr MultiFrame::getCurrentFrame() const {
	return frames_[currentFrame_];
}

