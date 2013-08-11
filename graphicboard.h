#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisboard.h"

#include <mw/text.h>
#include <mw/color.h>

#include <string>

// Used to draw a tetris board and corresponding player info.
class GraphicBoard {
public:
	// tetrisBoard is saved as an reference. It must be kept in memory.
    GraphicBoard(const TetrisBoard& tetrisBoard);
    ~GraphicBoard();

	// Sets info about how many rows been cleared by the player.
    void setNbrOfClearedRows(int nbr);
    void setPoints(int points);
    void setLevel(int level);
    void setName(std::string name);
	void setGameOverMessage(std::string message);
	void setCountDownMessage(std::string countDown);

	// Draws player info and the board. Using opengl.
	void draw();

	// Returns the width the graphic requires to draw (draw()) everything.
	double getWidth() const;

	// Returns the height the graphic requires to draw (draw()) everything.
	double getHeight() const;

private:
    void drawBorder() const;
    void drawInfo();
    void drawPreviewBlock();
    void drawBoard();
    void drawBeginArea() const;
    void drawBlock(const Block& block, int maxRow);

    mw::Text name_, level_, points_, nbrOfClearedRows_;
	mw::Text gameOverMessage_, countDown_;	

	double pixlePerSquare_;
    const TetrisBoard& tetrisBoard_;

    mw::Color frameColor_;

    double height_; // Height for the board
	double voidHeight_; // Height between board and outside screen, up/down and to the left.
	double borderLineThickness_; // Line thickness for the border.
	double previwBorderSizeInSquares_; // The preview border size in number of squares.
	double horizontalDistanceToText_; // Text horizontal distance between board and text.
};

#endif // GRAPHICBOARD_H
