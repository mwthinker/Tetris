#ifndef RAWTETRISBOARD_H
#define RAWTETRISBOARD_H

#include "block.h"

#include <vector>

// The events triggered by the tetris board.
enum class GameEvent : char {
	RESTARTED,
	BLOCK_COLLISION,
	CURRENT_BLOCK_UPDATED,
	NEXT_BLOCK_UPDATED,
	EXTERNAL_ROWS_ADDED,
	PLAYER_ROTATES_BLOCK,
	PLAYER_MOVES_BLOCK_LEFT,
	PLAYER_MOVES_BLOCK_RIGHT,
	PLAYER_MOVES_BLOCK_DOWN,
	GRAVITY_MOVES_BLOCK,
	ONE_ROW_REMOVED,
	TWO_ROW_REMOVED,
	THREE_ROW_REMOVED,
	FOUR_ROW_REMOVED,
	GAME_OVER
};

// Defines all valid ways of controlling the falling block.
enum class Move : char {
	ROTATE_LEFT,
	ROTATE_RIGHT,
	DOWN_GRAVITY,
	DOWN,
	LEFT,
	RIGHT,
	GAME_OVER
};

// Represents a tetris board.
class RawTetrisBoard {
public:
	RawTetrisBoard(int nbrRows, int nbrColumns, BlockType current, BlockType next);
	
	inline virtual ~RawTetrisBoard() {
	}

	// Move the block. The board will stay constant if game over is true.
    void update(Move move);

	// Update the next block to be. Triggers the game event NEXT_BLOCK_UPDATED.
	void updateNextBlock(BlockType next);

	// Update the current block to be. Triggers the gameevent CURRENT_BLOCK_UPDATED.
	// The current block is placed at the start position on the board.
	void updateCurrentBlock(BlockType current);

	void updateRestart(BlockType current, BlockType next);
    
	// Return the number of rows.
	inline int getRows() const {
		return nbrRows_;
	}

	// Return the number of columns.
	inline int getColumns() const {
		return nbrColumns_;
	}

	// Return true if the game is over else false.
	inline bool isGameOver() const {
		return isGameOver_;
	}

	// Return all non moving squares on the board. Index 0 to (rows+4)*columns-1.
	// All squares are saved in row major order and in ascending order.
    const std::vector<BlockType>& getBoardVector() const;

	// Return the moving block.
	inline Block getBlock() const {
		return current_;
	}

	// Return the type of the moving block.
	inline BlockType getBlockType() const {
		return current_.blockType();
	}

	// Return the type of the next block.
	inline BlockType getNextBlockType() const {
		return next_;
	}

	// Return the blocktype for a given position.
	inline BlockType getBlockType(int row, int column) const {
		if (row >= 0 && row < nbrRows_ && column >= 0 && column < nbrColumns_) {
			return gameboard_[row * nbrColumns_ + column];
		}
		return BlockType::WALL;
	}

	// Return true if the block is outside or on an already occupied square on the board.
	// Otherwise it return false.
	bool collision(const Block& block) const;

	inline char getRemovedRow1() const {
		return row1_;
	}

	inline char getRemovedRow2() const {
		return row2_;
	}

	inline char getRemovedRow3() const {
		return row3_;
	}

	inline char getRemovedRow4() const {
		return row4_;
	}

protected:
	// Set all squares on the board to empty.
	// Game over is set to false.
	void clearBoard();

	inline BlockType& blockType(int row, int column) {
		return gameboard_[row * nbrColumns_ + column];
	}

	Block createBlock(BlockType blockType) const;

private:
	// Is called just after the event given is triggered.
	// e.g. if the row removed event is triggered (this function is called), the board has
	// already removed the row mentioned.
	virtual void triggerEvent(GameEvent gameEvent) {
	}

	virtual std::vector<BlockType> addExternalRows() {
		return std::vector<BlockType>(0); // A vector with no dynamic allocated size.
	}

	// Add a block directly to the board.
	// I.e. The block become a part of the static board.
	void addBlockToBoard(const Block& block);

    int removeFilledRows(const Block& block);
    void moveRowsOneStepDown(int rowToRemove);
    
	std::vector<BlockType> gameboard_;	// Containing all non moving squares on the board.
	BlockType next_;					// Next block for the player to control.
	Block current_;						// The current block for the player to control.
	int nbrRows_, nbrColumns_;			// The size of the gameboard.
	bool isGameOver_;					// True when game is over, else false.
	char row1_, row2_, row3_, row4_;	// The last rows that was removed.
};

#endif // RAWTETRISBOARD_H
