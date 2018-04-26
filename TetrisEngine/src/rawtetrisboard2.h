#ifndef RAWTETRISBOARD2_H
#define RAWTETRISBOARD2_H

#include "block2.h"

#include <vector>
#include <cstdint>
#include <array>

// The events triggered by the tetris board.
enum class GameEvent2 {
	RESTARTED,
	BLOCK_COLLISION,
	CURRENT_BLOCK_UPDATED,
	NEXT_BLOCK_UPDATED,
	EXTERNAL_ROWS_ADDED,
	PLAYER_MOVES_BLOCK_ROTATE,
	PLAYER_MOVES_BLOCK_LEFT,
	PLAYER_MOVES_BLOCK_RIGHT,
	PLAYER_MOVES_BLOCK_DOWN,
	PLAYER_MOVES_BLOCK_DOWN_GROUND,
	GRAVITY_MOVES_BLOCK,
	ONE_ROW_REMOVED,
	TWO_ROW_REMOVED,
	THREE_ROW_REMOVED,
	FOUR_ROW_REMOVED,
	ROW_TO_BE_REMOVED,
	GAME_OVER
};

// Defines all valid ways of controlling the falling block.
enum class Move2 {
	ROTATE_LEFT,
	ROTATE_RIGHT,
	DOWN_GRAVITY,
	DOWN,
	DOWN_GROUND,
	LEFT,
	RIGHT,
	GAME_OVER
};

// Represents a tetris board.
class RawTetrisBoard2 {
public:
	RawTetrisBoard2(int columns, int rows, BlockType current, BlockType next);
	RawTetrisBoard2(const std::vector<BlockType>& board,
		int columns, int rows, const Block2& current, BlockType next);

	virtual ~RawTetrisBoard2() = default;

	// Move the block. The board will stay constant if game over is true.
	void update(Move2 move);

	// Update the next block to be. Triggers the game event NEXT_BLOCK_UPDATED.
	void updateNextBlock(BlockType next);

	// Update the current block to be. Triggers the gameevent CURRENT_BLOCK_UPDATED.
	// The current block is placed at the start position on the board.
	void updateCurrentBlock(BlockType current);

	void updateRestart(BlockType current, BlockType next);

	void updateRestart(int column, int row, BlockType current, BlockType next);

	// Return the number of rows.
	int getRows() const {
		return rows_;
	}

	// Return the number of columns.
	int getColumns() const {
		return columns_;
	}

	// Return true if the game is over else false.
	bool isGameOver() const {
		return isGameOver_;
	}

	// Return all non moving squares on the board. Index 0 to (rows+4)*columns-1.
	// All squares are saved in row major order and in ascending order.
	const std::vector<BlockType>& getBoardVector() const;

	// Return the moving block.
	Block2 getBlock() const {
		return current_;
	}

	// Return the type of the moving block.
	BlockType getBlockType() const {
		return current_.getBlockType();
	}

	// Return the type of the next block.
	BlockType getNextBlockType() const {
		return next_;
	}

	// Return the blocktype for a given position.
	BlockType getBlockType(int column, int row) const;

	// Return true if the block is outside or on an already occupied square on the board.
	// Otherwise it return false.
	bool collision(const Block2& block) const;

	int getNbrExternalRowsAdded() const {
		return externalRowsAdded_;
	}

	int getRowToBeRemoved() const {
		return rowToBeRemoved_;
	}

private:
	uint16_t extractBoardSegment(int lowX, int lowY) const;

	BlockType& board(int column, int row) {
		return gameboard_[row * columns_ + column];
	}

	BlockType board(int column, int row) const {
		return gameboard_[row * columns_ + column];
	}

	Block2 createBlock(BlockType blockType) const;

	bool isRowEmpty(int row) const {
		for (int column = 0; column < columns_; ++column) {
			if (board(column, row) != BlockType::EMPTY) {
				return false;
			}
		}
		return true;
	}

	bool isRowFilled(int row) const {
		for (int column = 0; column < columns_; ++column) {
			if (board(column, row) == BlockType::EMPTY) {
				return false;
			}
		}
		return true;
	}

	// Set all squares on the board to empty.
	// Game over is set to false.
	void clearBoard();

	// Is called just after the event given is triggered.
	// e.g. if the row removed event is triggered (this function is called), the board has
	// already removed the row mentioned.
	virtual void triggerEvent(GameEvent2 gameEvent) {
	}

	virtual std::vector<BlockType> addExternalRows() {
		return std::vector<BlockType>(0); // A vector with no dynamic allocated size.
	}

	// Add a block directly to the board.
	// I.e. The block become a part of the static board.
	void addBlockToBoard(const Block2& block);

	int removeFilledRows(const Block2& block);
	void moveRowsOneStepDown(int rowToRemove);

	std::vector<BlockType> gameboard_;	// Containing all non moving squares on the board.
	BlockType next_;					// Next block for the player to control.
	Block2 current_;					// The current block for the player to control.
	int columns_, rows_;				// The size of the gameboard.
	bool isGameOver_;					// True when game is over, else false.
	int externalRowsAdded_;
	int rowToBeRemoved_;
};

class Board {
public:
	Board() = default;

	Board(int width, int height) : gameboard_(height, 0),
		width_(width), height_(height) {
	}

	Board(const std::vector<BlockType>& board,
		int width, int height) : gameboard_(board.size() / width_ + 1), width_(width), height_(height) {

		const int size = board.size();
		for (int y = 0; y < (size / width_ + 1); ++y) {
			uint32_t row = 0;
			for (int x = 0; x < width_ && getIndex(x, y) < size; ++x) {
				if (board[getIndex(x, y)] != BlockType::EMPTY) {
					gameboard_[getIndex(x, y)] &= (1 << width_);
				}
			}
		}
	}

	void addBlockToBoard(Block2 block) {
		gameboard_[block.getLowY() + 3] &= ((block.getBits() & 0xF000) << block.getLowX());
		gameboard_[block.getLowY() + 2] &= ((block.getBits() & 0x0F00) << block.getLowX());
		gameboard_[block.getLowY() + 1] &= ((block.getBits() & 0x00F0) << block.getLowX());
		gameboard_[block.getLowY() + 0] &= ((block.getBits() & 0x000F) << block.getLowX());
	}

	bool collision(Block2& block) const {
		return false;
	}

	int getIndex(int x, int y) const {
		return x + y * width_;
	}

	void removeRow(int row1, int row2, int row3, int row4) {
		if (row1 >= 0 && row1 < height_) {
			int row = 0;
			auto it = std::remove_if(gameboard_.begin(), gameboard_.end(), [&](uint32_t value) {
				return true;
			});
		}
	}

	bool isRowFilled(int row) const {
		return gameboard_[row] > 0;
	}

private:
	int removeFilledRows(Block2 block) {
		return 0;
	}

	int width_, height_;
	std::vector<uint32_t> gameboard_;
};

/*
template <class EventListener>
	void update(Move2 move, EventListener& eventListener) {
		// Game over?
		if (collision(block_)) {
			eventListener(GameEvent2::GAME_OVER);
		} else {
			Block2 block = block_;
			switch (move) {
				case Move2::GAME_OVER:
					eventListener(GameEvent2::GAME_OVER);
					break;
				case Move2::LEFT:
					block.moveLeft();
					if (!collision(block)) {
						block_ = block;
						eventListener(GameEvent2::PLAYER_MOVES_BLOCK_LEFT);
					}
					break;
				case Move2::RIGHT:
					block.moveRight();
					if (!collision(block)) {
						block_ = block;
						eventListener(GameEvent2::PLAYER_MOVES_BLOCK_RIGHT);
					}
					break;
				case Move2::DOWN_GROUND:
					eventListener(GameEvent2::PLAYER_MOVES_BLOCK_DOWN_GROUND);
					do {
						block_ = block;
						block_.moveDown();
					} while (!collision(block));
					eventListener(GameEvent2::PLAYER_MOVES_BLOCK_DOWN);
					break;
				case Move2::DOWN:
					block.moveDown();
					if (!collision(block)) {
						block_ = block;
						eventListener(GameEvent2::PLAYER_MOVES_BLOCK_DOWN);
					}
					break;
				case Move2::ROTATE_RIGHT:
					block.rotateRight();
					if (!collision(block)) {
						block_ = block;
						eventListener(GameEvent2::PLAYER_MOVES_BLOCK_ROTATE);
					}
					break;
				case Move2::ROTATE_LEFT:
					block.rotateLeft();
					if (!collision(block)) {
						block_ = block;
						eventListener(GameEvent2::PLAYER_MOVES_BLOCK_ROTATE);
					}
					break;
				case Move2::DOWN_GRAVITY:
					block.moveDown();
					if (collision(block)) {
						// Collision detected, add squares to the gameboard.
						addBlockToBoard(block_);

						eventListener(GameEvent2::BLOCK_COLLISION);

						// Remove any filled row on the gameboard.
						int nbr = removeFilledRows(block_);

						switch (nbr) {
							case 1:
								eventListener(GameEvent2::ONE_ROW_REMOVED);
								break;
							case 2:
								eventListener(GameEvent2::TWO_ROW_REMOVED);
								break;
							case 3:
								eventListener(GameEvent2::THREE_ROW_REMOVED);
								break;
							case 4:
								eventListener(GameEvent2::FOUR_ROW_REMOVED);
								break;
						}
					} else {
						block_ = block;
						eventListener(GameEvent2::GRAVITY_MOVES_BLOCK);
					}
					break;
			}
		}
	}
*/

#endif // RAWTETRISBOARD2_H
