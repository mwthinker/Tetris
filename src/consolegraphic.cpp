#include "consolegraphic.h"

#include <chrono>
#include <map>

#include <console.h>

ConsoleGraphic::ConsoleGraphic() : tetrisBoard_(20, 10, BlockType::I, BlockType::I), x_(0), y_(0), console_(nullptr) {
}

ConsoleGraphic::~ConsoleGraphic() {
	connection_.disconnect();
}

void ConsoleGraphic::restart(Player& player, TetrisEntry tetrisEntry, int x, int y, console::Console* console) {
	console_ = console;
	x_ = x;
	y_ = y;
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
	switch (gameEvent) {
	case GameEvent::EXTERNAL_ROWS_ADDED:
	case GameEvent::CURRENT_BLOCK_UPDATED:
	case GameEvent::PLAYER_MOVES_BLOCK_DOWN:
	case GameEvent::PLAYER_MOVES_BLOCK_DOWN_GROUND:
	case GameEvent::PLAYER_MOVES_BLOCK_LEFT:
	case GameEvent::PLAYER_MOVES_BLOCK_RIGHT:
	case GameEvent::GRAVITY_MOVES_BLOCK:
		drawBoard();
		break;
	case GameEvent::NEXT_BLOCK_UPDATED:
		drawNextBlock();
		break;
	}
}

void ConsoleGraphic::drawNextBlock() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	for (int row = 0; row < 5; ++row) {
		draw(columns + 6, 2 + row, "     ");
	}

	Block nextBlock = Block(tetrisBoard_.getNextBlockType(), 0, 0);
	for (const Square& sq : nextBlock) {
		drawSquare(9 + columns - sq.column_, 3 + sq.row_, sq.blockType_);
	}
}

void ConsoleGraphic::drawCurrentBlock() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();
	
	Block currentBlock = tetrisBoard_.getBlock();
	for (const Square& sq : currentBlock) {
		drawSquare(1 + columns - sq.column_, 1 + rows - sq.row_, sq.blockType_);
	}
}

void ConsoleGraphic::drawBoard() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	// Clear board.
	std::string remove = "";
	for (int i = 0; i < columns; ++i) {
		remove += " ";
	}
	for (int row = 0; row < rows - 1; ++row) {
		draw(2, row - 1, remove);
	}

	// Draw current block.
	Block currentBlock = tetrisBoard_.getBlock();
	for (const Square& sq : currentBlock) {
		drawSquare(1 + columns - sq.column_, rows - sq.row_ - 3, sq.blockType_);
	}
	
	// Draw tetris board.
	for (int row = 0; row < rows; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType blockType = tetrisBoard_.getBlockType(row, column);
			drawSquare(1 + columns - column, rows - row - 3, blockType);
		}
	}
}

void ConsoleGraphic::drawStatic() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	// Draw frame around next block.
	//rlutil::setColor(rlutil::WHITE);
	for (int row = 0; row < 7; ++row) {
		draw(columns + 5, 1 + row, SQUARE);
		draw(columns + 5 + 7, 1 + row, SQUARE);
	}
	for (int column = 0; column < 8; ++column) {
		draw(columns + column + 5, 1, SQUARE);
		draw(columns + column + 5, 1 + 7, SQUARE);
	}

	// Draw frame around tetris board.
	//rlutil::setColor(rlutil::WHITE);
	for (int row = 0; row < rows + 1; ++row) {
		draw(1, 1 + row, SQUARE);
		draw(2 + columns, 1 + row, SQUARE);
	}
	for (int column = 0; column < columns + 2; ++column) {
		draw(1 + column, 2 + rows, SQUARE);
	}

	drawNextBlock();
	drawCurrentBlock();
	drawText();
}

void ConsoleGraphic::drawText() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	draw(columns + 5, 0, "              ");
	draw(columns + 5, 0, playerName_);
	draw(columns + 5, 10, "              ");
	draw(columns + 5, 10, "Rows: ", tetrisBoard_.getRows());
}

void ConsoleGraphic::drawSquare(int x, int y, BlockType blockType) const {
	console_->setCursorPosition(x_ + x, y_ + y);
	switch (blockType) {
		case BlockType::I:
			console_->setTextColor(console::Color::CYAN);
			*console_ << SQUARE;
			break;
		case BlockType::J:
			console_->setTextColor(console::Color::BLUE);
			*console_ << SQUARE;
			break;
		case BlockType::L:
			console_->setTextColor(console::Color::RED);
			*console_ << SQUARE;
			break;
		case BlockType::O:
			console_->setTextColor(console::Color::YELLOW);
			*console_ << SQUARE;
			break;
		case BlockType::S:
			console_->setTextColor(console::Color::RED);
			*console_ << SQUARE;
			break;
		case BlockType::T:
			console_->setTextColor(console::Color::MAGENTA);
			*console_ << SQUARE;
			break;
		case BlockType::Z:
			console_->setTextColor(console::Color::GREEN);
			*console_ << SQUARE;
			break;
	}
}

void ConsoleGraphic::draw(int x, int y, char key) const {
	if (x_ + x > 0 && y_ + y > 0) {
		console_->setCursorPosition(x_ + x, y_ + y);
		*console_ << key;
	}
}

void ConsoleGraphic::draw(int x, int y, std::string text) const {
	if (x_ + x > 0 && y_ + y > 0) {
		console_->setCursorPosition(x_ + x, y_ + y);
		*console_ << text;
	}
}

void ConsoleGraphic::draw(int x, int y, std::string text, int number) const {
	if (x_ + x > 0 && y_ + y > 0) {
		console_->setCursorPosition(x_ + x, y_ + y);
		*console_ << text << number;
	}
}
