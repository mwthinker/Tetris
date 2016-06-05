#include "rawtetrisboard.h"
#include "square.h"
#include "block.h"

RawTetrisBoard::RawTetrisBoard(int rows, int columns,
	BlockType current, BlockType next) :
	gameboard_(rows * columns, BlockType::EMPTY),
	next_(next),
	rows_(rows), columns_(columns),
	isGameOver_(false) {
	
	current_ = createBlock(current);
}

void RawTetrisBoard::update(Move move) {
	// Game over?
	if (isGameOver_ || collision(current_)) {
		if (!isGameOver_) {
			// Only called once when the game becomes game over.
			isGameOver_ = true;
			triggerEvent(GameEvent::GAME_OVER);
		}
	} else {
		Block block = current_;
		switch (move) {
			case Move::GAME_OVER:
				// Only called once when the game becomes game over.
				isGameOver_ = true;
				triggerEvent(GameEvent::GAME_OVER);
				break;
			case Move::LEFT:
				block.moveLeft();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_LEFT);
				}
				break;
			case Move::RIGHT:
				block.moveRight();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_RIGHT);
				}
				break;
			case Move::DOWN:
				block.moveDown();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_DOWN);
				}
				break;
			case Move::ROTATE_RIGHT:
				block.rotateRight();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_ROTATES_BLOCK);
				}
				break;
			case Move::ROTATE_LEFT:
				block.rotateLeft();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_ROTATES_BLOCK);
				}
				break;
			case Move::DOWN_GRAVITY:
				block.moveDown();
				if (collision(block)) {
					// Collision detected, add squares to the gameboard.
					addBlockToBoard(current_);

					triggerEvent(GameEvent::BLOCK_COLLISION);

					// Remove any filled row on the gameboard.
					int nbr = removeFilledRows(current_);

					// Add rows due to some external event.
					std::vector<BlockType> squares = addExternalRows();
					if (squares.size() > 0) {
						externalRowsAdded_ = squares.size() % columns_;
						gameboard_.insert(gameboard_.begin(), squares.begin(), squares.end());
						gameboard_.resize(gameboard_.size() - squares.size(), BlockType::EMPTY);
						triggerEvent(GameEvent::EXTERNAL_ROWS_ADDED);
					}

					// Update the user controlled block.
					current_ = createBlock(next_);
					triggerEvent(GameEvent::CURRENT_BLOCK_UPDATED);

					switch (nbr) {
						case 1:
							triggerEvent(GameEvent::ONE_ROW_REMOVED);
							break;
						case 2:
							triggerEvent(GameEvent::TWO_ROW_REMOVED);
							break;
						case 3:
							triggerEvent(GameEvent::THREE_ROW_REMOVED);
							break;
						case 4:
							triggerEvent(GameEvent::FOUR_ROW_REMOVED);
							break;
					}
				} else {
					current_ = block;
					triggerEvent(GameEvent::GRAVITY_MOVES_BLOCK);
				}
				break;
		}
	}
}

void RawTetrisBoard::updateNextBlock(BlockType nextBlock) {
	next_ = nextBlock;
	triggerEvent(GameEvent::NEXT_BLOCK_UPDATED);
}

void RawTetrisBoard::updateCurrentBlock(BlockType current) {
	current_ = createBlock(current);
	triggerEvent(GameEvent::CURRENT_BLOCK_UPDATED);
}

void RawTetrisBoard::updateRestart(BlockType current, BlockType next) {
	updateRestart(rows_, columns_, current, next);
}

void RawTetrisBoard::updateRestart(int rows, int columns, BlockType current, BlockType next) {
	next_ = next;
	current_ = createBlock(current);
	rows_ = rows;
	columns_ = columns;
	clearBoard();
	gameboard_.resize(rows * columns, BlockType::EMPTY);
	triggerEvent(GameEvent::RESTARTED);
}

const std::vector<BlockType>& RawTetrisBoard::getBoardVector() const {
	return gameboard_;
}

void RawTetrisBoard::addBlockToBoard(const Block& block) {
	// All squares in the block is added to the gameboard.
	for (const Square& sq : block) {
		blockType(sq.row_, sq.column_) = sq.blockType_;
	}
}

Block RawTetrisBoard::createBlock(BlockType blockType) const {
	return Block(blockType, rows_ - 4, columns_ / 2 - 1); // 4 rows are the starting area.
}

bool RawTetrisBoard::collision(const Block& block) const {
	bool collision = false;

	for (const Square& sq : block) {
		if (getBlockType(sq.row_, sq.column_) != BlockType::EMPTY) {
			collision = true;
			break;
		}
	}

	return collision;
}

void RawTetrisBoard::clearBoard() {
	gameboard_.assign(gameboard_.size(), BlockType::EMPTY);
	isGameOver_ = false;
}

int RawTetrisBoard::removeFilledRows(const Block& block) {
	row1_ = -1;
	row2_ = -1;
	row3_ = -1;
	row4_ = -1;
	int row = block.getLowestRow();

	int nbr = 0; // Number of rows filled.
	const int nbrOfSquares = current_.nbrOfSquares();
	for (int i = 0; i < nbrOfSquares; ++i) {
		bool filled = true;
		if (row >= 0) {
			int index = row * columns_;
			for (int column = 0; column < columns_; ++column) {
				if (gameboard_[index + column] == BlockType::EMPTY) {
					filled = false;
				}
			}
		} else {
			filled = false;
		}
		if (filled) {
			moveRowsOneStepDown(row);
			// Which row was filled?
			switch (nbr) {
				case 0:
					row1_ = row + nbr;
					break;
				case 1:
					row2_ = row + nbr;;
					break;
				case 2:
					row3_ = row + nbr;;
					break;
				case 3:
					row4_ = row + nbr;;
					break;
			}
			++nbr;
		} else {
			++row;
		}
	}

	return nbr;
}

void RawTetrisBoard::moveRowsOneStepDown(int rowToRemove) {
	int indexStartOfRow = rowToRemove * columns_;
	// Erase the row.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.begin() + indexStartOfRow + columns_);
	// Replace the removed row with an empty row at the top.
	gameboard_.insert(gameboard_.end(), columns_, BlockType::EMPTY);
}