#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "color.h"
#include "tetrisboard.h"
#include "drawboard.h"

#include <mw/text.h>

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

	// Draws player info and the board. Using opengl.
    void draw();

	// Returns the width the graphic requires to draw (draw()) everything.
    double getWidth() const;
    
	// Returns the height the graphic requires to draw (draw()) everything.
	double getHeight() const;
private:
    void drawBorder();
    void drawInfo();
    void drawPreviewBlock();
    void drawBoard();
    void drawBeginArea();
    void drawBlock(const Block& block);
    void drawSquare(const Square& square);
    void drawSquare(BlockType blockType);
    void drawGrid();
    void drawGridSquare();
    void glColorWhite() const;
    void drawFrame(double x1, double y1, double x2, double y2, double width);
    
    mw::Text text1_;
    mw::Text text2_;
    mw::Text text3_;
    mw::Text text4_;
    
	mw::Text gameOverMessage_;	
	
	double pixlePerSquare_;
    const TetrisBoard& tetrisBoard_;
    
    Color color_;

    double height_; // Height for the board
	double voidHeight_; // Height between board and outside screen, up/down and to the left.
	double borderLineThickness_; // Line thickness for the border. 
	double previwBorderSizeInSquares_; // The preview border size in number of squares.
	double horizontalDistanceToText_; // Text horizontal distance between board and text.

    int nbrOfClearedRows_, points_, level_;
    std::string name_;
};

#endif // GRAPHICBOARD_H
