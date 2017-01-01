#include "consolegraphic.h"

#include <chrono>
#include <map>

#include <rlutil.h>

ConsoleGraphic::ConsoleGraphic() : tetrisBoard_(20, 10, BlockType::I, BlockType::I) {
}

ConsoleGraphic::~ConsoleGraphic() {
	connection_.disconnect();
}

void ConsoleGraphic::restart(Player& player, TetrisEntry tetrisEntry) {
	connection_.disconnect();
	connection_ = player.addGameEventListener(std::bind(&ConsoleGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));

	playerName_ = "Player";
}

int ConsoleGraphic::getWidth() const {
	return tetrisBoard_.getColumns() + 14;
}

int ConsoleGraphic::getHeight() const {
	return tetrisBoard_.getRows() + 5;
}

void ConsoleGraphic::callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	tetrisBoard_ = tetrisBoard;
}

void ConsoleGraphic::draw(double time, int x, int y) {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	// Draw frame around next block.
	rlutil::setColor(rlutil::WHITE);
	for (int row = 0; row < 7; ++row) {
		draw(x + columns + 5, y + 1 + row, SQUARE);
		draw(x + columns + 5 + 7, y + 1 + row, SQUARE);
	}
	for (int column = 0; column < 8; ++column) {
		draw(x + columns + column + 5, y + 1, SQUARE);
		draw(x + columns + column + 5, y + 1 + 7, SQUARE);
	}
	Block nextBlock = Block(tetrisBoard_.getNextBlockType(), 0, 0);
	for (const Square& sq : nextBlock) {
		drawSquare(x + 9 + columns - sq.column_, y + 3 + sq.row_, sq.blockType_);
	}

	// Draw frame around tetris board.
	rlutil::setColor(rlutil::WHITE);
	for (int row = 0; row < rows + 1; ++row) {
		draw(x + 1, y + 1 + row, SQUARE);
		draw(x + 2 + columns, y + 1 + row, SQUARE);
	}
	for (int column = 0; column < columns + 2; ++column) {
		draw(x + 1 + column, y + 2 + rows, SQUARE);
	}

	// Draw tetris board.
	for (int row = 0; row < rows; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType blockType = tetrisBoard_.getBlockType(row, column);
			drawSquare(x + 1 + columns - column, y + 1 + rows - row, blockType);
		}
	}

	// Draw current block.
	Block currentBlock = tetrisBoard_.getBlock();
	for (const Square& sq : currentBlock) {
		drawSquare(x + 1 + columns - sq.column_, y + 1 + rows - sq.row_, sq.blockType_);
	}

	draw(x + columns + 5, y, playerName_);
	draw(x + columns + 5, y + 10, "Rows: ", tetrisBoard_.getRows());
}

void ConsoleGraphic::drawSquare(int x, int y, BlockType blockType) {
	rlutil::locate(x, y);
	switch (blockType) {
		case BlockType::I:
			rlutil::setColor(rlutil::CYAN);
			std::cout << SQUARE;
			break;
		case BlockType::J:
			rlutil::setColor(rlutil::BLUE);
			std::cout << SQUARE;
			break;
		case BlockType::L:
			rlutil::setColor(rlutil::LIGHTRED);
			std::cout << SQUARE;
			break;
		case BlockType::O:
			rlutil::setColor(rlutil::YELLOW);
			std::cout << SQUARE;
			break;
		case BlockType::S:
			rlutil::setColor(rlutil::RED);
			std::cout << SQUARE;
			break;
		case BlockType::T:
			rlutil::setColor(rlutil::MAGENTA);
			std::cout << SQUARE;
			break;
		case BlockType::Z:
			rlutil::setColor(rlutil::GREEN);
			std::cout << SQUARE;
			break;
		default:
			break;
	}
}

void ConsoleGraphic::draw(int x, int y, char key) {
	rlutil::locate(x, y);
	std::cout << key;
}

void ConsoleGraphic::draw(int x, int y, std::string text) {
	rlutil::locate(x, y);
	std::cout << text;
}

void ConsoleGraphic::draw(int x, int y, std::string text, int number) {
	rlutil::locate(x, y);
	std::cout << text << number;
}
