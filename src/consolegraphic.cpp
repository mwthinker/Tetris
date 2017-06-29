#include "consolegraphic.h"

#include <chrono>
#include <map>
#include <iostream>

#include <console/console.h>

const std::string ConsoleGraphic::SQUARE = "  ";

ConsoleGraphic::ConsoleGraphic() : tetrisBoard_(20, 10, randomBlockType(), randomBlockType()), x_(0), y_(0), console_(nullptr) {
}

ConsoleGraphic::~ConsoleGraphic() {
	connection_.disconnect();
}

void ConsoleGraphic::restart(Player& player, int x, int y, console::Console* console) {
	console_ = console;
	x_ = x + 1;
	y_ = y + 3;
	connection_.disconnect();
	connection_ = player.addGameEventListener(std::bind(&ConsoleGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));
	playerName_ = "Player";
	points_ = 0;
	level_ = 1;
}

int ConsoleGraphic::getWidth() const {
	return tetrisBoard_.getColumns() + 34;
}

int ConsoleGraphic::getHeight() const {
	return tetrisBoard_.getRows() + 5;
}

void ConsoleGraphic::callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	tetrisBoard_ = tetrisBoard;
	switch (gameEvent) {
	case GameEvent::RESTARTED:
	case GameEvent::EXTERNAL_ROWS_ADDED:
	case GameEvent::CURRENT_BLOCK_UPDATED:
	case GameEvent::PLAYER_MOVES_BLOCK_DOWN:
	case GameEvent::PLAYER_MOVES_BLOCK_DOWN_GROUND:
	case GameEvent::PLAYER_MOVES_BLOCK_LEFT:
	case GameEvent::PLAYER_MOVES_BLOCK_ROTATE:
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

	console_->setBackgroundColor(console::Color::BLACK);
	for (int row = 0; row < 6; ++row) {
		draw((columns + 3) * 2, 2 + row, "            ");
	}

	Block nextBlock = Block(tetrisBoard_.getNextBlockType(), 0, 0);
	for (const Square& sq : nextBlock) {
		drawSquare((6 + columns - sq.column_) * 2, 3 + sq.row_, sq.blockType_);
	}
}

void ConsoleGraphic::drawCurrentBlock() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();
	
	Block currentBlock = tetrisBoard_.getBlock();
	int boardheight = tetrisBoard_.getRows();
	for (const Square& sq : currentBlock) {
		if (sq.row_ < boardheight - 2) {
			drawSquare(1 + sq.column_ * 2, rows - sq.row_ - 4, sq.blockType_);
		}
	}
}

void ConsoleGraphic::drawBoard() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	// Clear board.
	std::string remove = "";
	for (int i = 0; i < columns; ++i) {
		remove += "  ";
	}
	for (int row = 4; row < rows+2; ++row) {
		draw(1, row - 5, remove);
	}

	// Draw current block.
	drawCurrentBlock();
	
	// Draw tetris board.
	for (int row = 0; row < rows + 4; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType blockType = tetrisBoard_.getBlockType(row, column);
			drawSquare(1 + column * 2, rows - row - 4, blockType);
		}
	}
}

void ConsoleGraphic::updatePoints(int points) {
	points_ = points;
	drawText();
}

void ConsoleGraphic::updateLevel(int level) {
	level_ = level;
	drawText();
}

void ConsoleGraphic::drawStatic() const {
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	// Draw frame around next block.
	console_->setBackgroundColor(console::Color::WHITE);
	for (int row = 0; row < 7; ++row) {
		draw((columns + 2) * 2, 1 + row, SQUARE);
		draw((columns + 2 + 7) * 2, 1 + row, SQUARE);
	}
	for (int column = 0; column < 8; ++column) {
		draw((columns + column + 2) * 2, 1, SQUARE);
		draw((columns + column + 2) * 2, 1 + 7, SQUARE);
	}

	// Draw frame around tetris board.
	// Draw the sides.
	
	console_->setBackgroundColor(console::Color::WHITE);
	for (int row = 0; row < rows + 2; ++row) {
		draw(0, row - 1, " ");
		draw(columns * 2 + 1, row - 1, " ");
	}
	
	// Mark the block starting rows in red.
	console_->setTextColor(console::Color::BLACK);
	console_->setBackgroundColor(console::Color::RED);
	draw(0, -1, " ");
	draw(columns * 2 + 1, -1, " ");
	draw(0, 0, " ");
	draw(columns * 2 + 1, 0, " ");
	
	// Draw the lowest row.
	console_->setBackgroundColor(console::Color::WHITE);
	for (int column = 0; column < columns + 1; ++column) {
		draw(column * 2, rows + 1, SQUARE);
	}

	// Draw player border.
	console_->setBackgroundColor(console::Color::WHITE);
	for (int i = 0; i < getWidth() + 1; ++i) {
		draw(i - 2, -3, " ");
		draw(i - 2, -2 + getHeight(), " ");
	}
	for (int i = 0; i < getHeight(); ++i) {
		draw(-2, i - 2, " ");
		draw(getWidth() - 2, i - 2, " ");
	}
	
	drawNextBlock();
	drawCurrentBlock();
	drawText();
}

void ConsoleGraphic::drawText() const {
	console_->setTextColor(console::Color::WHITE);
	console_->setBackgroundColor(console::Color::BLACK);
	int rows = tetrisBoard_.getRows();
	int columns = tetrisBoard_.getColumns();

	draw(columns * 2 + 5, 0, "              ");
	draw(columns * 2 + 5, 0, playerName_);
	draw(columns * 2 + 5, 10, "              ");
	draw(columns * 2 + 5, 10, "Level: ", level_);
	draw(columns * 2 + 5, 11, "              ");
	draw(columns * 2 + 5, 12, "Points: ", points_);
	draw(columns * 2 + 5, 13, "              ");
	draw(columns * 2 + 5, 13, "Rows: ", tetrisBoard_.getRemovedRows());
}

void ConsoleGraphic::drawSquare(int x, int y, BlockType blockType) const {
	console_->setTextColor(console::Color::WHITE);
	console_->setCursorPosition(x_ + x, y_ + y);
	switch (blockType) {
		case BlockType::I:
			console_->setBackgroundColor(console::Color::CYAN);
			console_->print(SQUARE);
			break;
		case BlockType::J:
			console_->setBackgroundColor(console::Color::BLUE);
			console_->print(SQUARE);
			break;
		case BlockType::L:
			console_->setBackgroundColor(console::Color::RED);
			console_->print(SQUARE);
			break;
		case BlockType::O:
			console_->setBackgroundColor(console::Color::YELLOW);
			console_->print(SQUARE);
			break;
		case BlockType::S:
			console_->setBackgroundColor(console::Color::RED);
			console_->print(SQUARE);
			break;
		case BlockType::T:
			console_->setBackgroundColor(console::Color::MAGENTA);
			console_->print(SQUARE);
			break;
		case BlockType::Z:
			console_->setBackgroundColor(console::Color::GREEN);
			console_->print(SQUARE);
			break;
	}
	console_->setBackgroundColor(console::Color::BLACK);
}

void ConsoleGraphic::draw(int x, int y, char key) const {
	if (x_ + x > 0 && y_ + y > 0) {
		console_->setCursorPosition(x_ + x, y_ + y);
		console_->print(key);
	}
}

void ConsoleGraphic::draw(int x, int y, std::string text) const {
	if (x_ + x > 0 && y_ + y > 0) {
		console_->setCursorPosition(x_ + x, y_ + y);
		console_->print(text);
	}
}

void ConsoleGraphic::draw(int x, int y, std::string text, int number) const {
	if (x_ + x > 0 && y_ + y > 0) {
		console_->setCursorPosition(x_ + x, y_ + y);
		console_->print(text).print(number);
	}
}
