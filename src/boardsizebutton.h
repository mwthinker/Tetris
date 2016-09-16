#ifndef BOARDSIZEBUTTON_H
#define BOARDSIZEBUTTON_H

#include <gui/component.h>

#include <mw/sprite.h>

class BoardSizeButton : public gui::Component {
public:
	BoardSizeButton(int minX, int minY, int maxX, int maxY, const mw::Sprite& icon);

	int getColumns() const {
		return x_;
	}
	
	int getRows() const {
		return y_;
	}
	
	void draw(const gui::Graphic& graphic, double deltaTime) override;

private:
	void handleMouse(const SDL_Event& mouseEvent) override;
	void mouseMotionLeave() override;
	
	int x_, y_;
	int minX_, minY_, maxX_, maxY_;

	mw::Sprite icon_, iconDown_;
	mw::Sprite outerSquare_, innerSquare_;
	bool mouseInside_;
	bool pushed_;
};

#endif // BOARDSIZEBUTTON_H
