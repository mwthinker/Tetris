#ifndef MULTIFRAME_H
#define MULTIFRAME_H

#include "guitypedefs.h"

#include <vector>

#include <SDL.h>

namespace gui {

	class MultiFrame {
	public:
		MultiFrame();

		// Get the panel corrensponding to the frame with index (index).
		FramePtr operator[](int index);

		void unfocus(int indexFrame);

		// Creates a new frame at the back.
		int addFrameBack();
		void add(GuiItemPtr guiItem, int x, int y, bool invX, bool invY);

		// Add item (item) to frame with index (frameIndex).
		void add(GuiItemPtr guiItem, int x, int y, bool invX, bool invY, int frameIndex);

		// Is called in order to update all items in the active frame.
		void eventUpdate(const SDL_Event& windowEvent);
		void setBackground(BackgroundPtr background);
		void setBackground(BackgroundPtr background, int frameIndex);

		void addBar(BarPtr bar);
		void addBar(BarPtr bar, int frameIndex);

		void draw(double timeStep);
		int getNbrOfFrames() const;
		int getCurrentFrameIndex() const;
		void setCurrentFrame(int index);
		FramePtr getCurrentFrame() const;

	private:
		std::vector<FramePtr> frames_;
		int currentFrame_;
	};

} // Namespace gui.

#endif // MULTIFRAME_H
