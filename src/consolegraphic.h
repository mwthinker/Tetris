#ifndef CONSOLEGRAPHIC_H
#define CONSOLEGRAPHIC_H

#include "tetrisentry.h"
#include "tetrisgameevent.h"
#include "tetrisboard.h"

class ConsoleGraphic {
public:
	ConsoleGraphic();

	~ConsoleGraphic();

	void restart(Player& player, TetrisEntry tetrisEntry);

	int getWidth() const;

	void callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	void draw(double time, int x, int y);

	void drawSquare(int x, int y, BlockType blockType);

private:
	void draw(int x, int y, char key);

	void draw(int x, int y, std::string text);

	void draw(int x, int y, std::string text, int number);

	static const char SQUARE = (char) 219;

	mw::signals::Connection connection_;
	RawTetrisBoard tetrisBoard_;
	std::string playerName_;
};

#endif // CONSOLEGRAPHIC_H
