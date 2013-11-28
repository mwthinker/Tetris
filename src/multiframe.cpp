#include "multiframe.h"

#include "widget.h"
#include "bar.h"
#include "frame.h"

#include <mw/color.h>

#include <vector>
#include <SDL_opengl.h>

namespace gui {

	MultiFrame::MultiFrame(SDL_Window* window) {
		window_ = window;
		currentFrame_ = 0;
		addFrameBack();
	}

	// Creates a new frame at the back.
	int MultiFrame::addFrameBack() {
		FramePtr frame(new Frame(window_));
		frame->setBackground(defaultBackground_);
		frames_.push_back(frame);
		return frames_.size() - 1;
	}

	void MultiFrame::add(WidgetPtr guiItem, int x, int y, bool invX, bool invY) {
		frames_[currentFrame_]->add(guiItem, x, y, invX, invY);
		guiItem->window_ = window_;
	}

	// Add item (item) to frame with index (frameIndex).
	void MultiFrame::add(WidgetPtr guiItem, int x, int y, bool invX, bool invY, int frameIndex) {
		frames_[frameIndex]->add(guiItem, x, y, invX, invY);
		guiItem->window_ = window_;
	}

	// Is called in order to update all items in the active frame.
	void MultiFrame::eventUpdate(const SDL_Event& windowEvent) {
		frames_[currentFrame_]->eventUpdate(windowEvent);
	}

	void MultiFrame::setDefaultBackground(const BackgroundPtr& background) {
		// Check all frames if a background is set.
		for (FramePtr& frame : frames_) {
			// Background not set?
			if (!frame->getBackground()) {
				frame->setBackground(background);
			}
		}
		defaultBackground_ = background;
	}

	void MultiFrame::setBackground(const BackgroundPtr& background) {
		frames_[currentFrame_]->setBackground(background);
	}

	void MultiFrame::setBackground(const BackgroundPtr& background, int frameIndex) {
		frames_[frameIndex]->setBackground(background);
	}

	void MultiFrame::addBar(const BarPtr& bar) {
		frames_[currentFrame_]->addBar(bar);
	}

	void MultiFrame::addBar(const BarPtr& bar, int frameIndex) {
		frames_[frameIndex]->addBar(bar);
	}

	void MultiFrame::draw(double timeStep) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		int windowsWidth, windowsHeight;
		frames_[currentFrame_]->windowSize(windowsWidth, windowsHeight);
		glViewport(0, 0, windowsWidth, windowsHeight);
		glOrtho(0, windowsWidth, 0, windowsHeight, -1, 1);
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

} // Namespace gui.
