#ifndef MULTIFRAME_H
#define MULTIFRAME_H

#include "guitypedefs.h"

#include <vector>

#include <SDL.h>

namespace gui {

	class MultiFrame {
	public:
		MultiFrame();

		// Creates a new frame at the back. Returns the added frame's index.
		int addFrameBack();
		void add(const GuiItemPtr& guiItem, int x, int y, bool invX, bool invY);

		// Add item (item) to frame with index (frameIndex).
		void add(const GuiItemPtr& guiItem, int x, int y, bool invX, bool invY, int frameIndex);

		// Is called in order to update all items in the active frame.
		void eventUpdate(const SDL_Event& windowEvent);

		// Will be used automaticly in all frames as background if other background is not set.
		void setDefaultBackground(const BackgroundPtr& background);
		void setBackground(const BackgroundPtr& background);
		void setBackground(const BackgroundPtr& background, int frameIndex);

		void addBar(const BarPtr& bar);
		void addBar(const BarPtr& bar, int frameIndex);

		void draw(double timeStep);
		
		int getNbrOfFrames() const;
		int getCurrentFrameIndex() const;
		void setCurrentFrame(int index);
		FramePtr getCurrentFrame() const;

	private:
		std::vector<FramePtr> frames_;
		int currentFrame_;
		BackgroundPtr defaultBackground_;
	};

} // Namespace gui.

#endif // MULTIFRAME_H
