#ifndef CONSOLEGRAPHIC_H
#define CONSOLEGRAPHIC_H

#include "tetrisentry.h"
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

	void drawSquare(int x, int y, BlockType blockType) const;
	
	void drawStatic() const;

	void drawText() const;

	void updatePoints(int points);

	void updateLevel(int level);
private:
	void draw(int x, int y, char key) const;

	void draw(int x, int y, std::string text) const;

	void draw(int x, int y, std::string text, int number) const;

	void drawNextBlock() const;
	void drawCurrentBlock() const;
	void drawBoard() const;

	static const std::string SQUARE;

	mw::signals::Connection connection_;
	RawTetrisBoard tetrisBoard_;
	std::string playerName_;
	int x_, y_;
	int points_, level_;
	console::Console* console_;
};

#endif // CONSOLEGRAPHIC_H
