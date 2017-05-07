#ifndef CONSOLEGRAPHIC_H
#define CONSOLEGRAPHIC_H

#include "tetrisentry.h"
#include "tetrisgameevent.h"
#include "tetrisboard.h"

class ConsoleGraphic {
public:
	ConsoleGraphic();

	~ConsoleGraphic();

	void restart(Player& player, TetrisEntry tetrisEntry, int x, int y);

	int getWidth() const;

	int getHeight() const;

	void callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	void drawSquare(int x, int y, BlockType blockType) const;
	
	void drawStatic() const;

	void drawText() const;
private:
	void draw(int x, int y, char key) const;

	void draw(int x, int y, std::string text) const;

	void draw(int x, int y, std::string text, int number) const;

	void drawNextBlock() const;
	void drawCurrentBlock() const;
	void drawBoard() const;

	static const char SQUARE = 'X';

	mw::signals::Connection connection_;
	RawTetrisBoard tetrisBoard_;
	std::string playerName_;
	int x_, y_;
};

#endif // CONSOLEGRAPHIC_H
