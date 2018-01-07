#ifndef CONSOLEGRAPHIC_H
#define CONSOLEGRAPHIC_H

#include "tetrisgameevent.h"
#include "tetrisboard.h"

#include <console/console.h>

class ConsoleGraphic {
public:
	ConsoleGraphic();

	~ConsoleGraphic();

	void restart(Player& player, int x, int y, console::Console* console);

	int getWidth() const;

	int getHeight() const;

	void callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	void updatePoints(int points);

	void updateLevel(int level);

	void draw(const TetrisBoard& tetrisBoard);

	void drawStatic() const;

private:
	

	void drawText() const;
	
	void drawSquare(int x, int y, BlockType blockType) const;

	void draw(int x, int y, char key) const;

	void draw(int x, int y, std::string text) const;

	void draw(int x, int y, std::string text, int number) const;

	void drawNextBlock(BlockType nextBlockType) const;
	void drawCurrentBlock(const Block& currentBlock) const;
	void drawBoard(const TetrisBoard& board) const;

	static const std::string SQUARE;

	mw::signals::Connection connection_;
	std::string playerName_;
	int x_, y_;
	int rows_, columns_;
	int nbrRemovedRows_;
	BlockType nextBlockType_;
	Block currentBlock_;

	int points_, level_;
	console::Console* console_;
};

#endif // CONSOLEGRAPHIC_H
