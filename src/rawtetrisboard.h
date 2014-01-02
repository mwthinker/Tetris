#ifndef RAWTETRISBOARD_H
#define RAWTETRISBOARD_H

#include "block.h"

#include <vector>

enum class GameEvent : char {
	PLAYER_ROTATES_BLOCK,
	PLAYER_MOVES_BLOCK_LEFT,
	PLAYER_MOVES_BLOCK_RIGHT,
	PLAYER_MOVES_BLOCK_DOWN,
	GRAVITY_MOVES_BLOCK,
	BLOCK_COLLISION,
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
	RIGHT
};

// Represents a tetris board.
class RawTetrisBoard {
public:
    RawTetrisBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next);
	
	inline virtual ~RawTetrisBoard() {
	}

	// Triggers a gameover event to be added to the event queue.
	void triggerGameOverEvent();

	// Add a move to the falling block.
    void update(Move move);
    
	// Return the number of rows.
	int getNbrOfRows() const;
    
	// Return the number of columns.
	int getNbrOfColumns() const;
	
	// Return true if the game is over else false.
    bool isGameOver() const;

	// Returns all non moving squares on the board. Index 0 to (rows+4)*columns-1.
	// All squares are saved in row major order.
    const std::vector<BlockType>& getBoardVector() const;

	// Return the moving block.
    Block currentBlock() const;

	// Return the next moving block.
    Block nextBlock() const;

	inline BlockType getBlockType(int row, int column) const {
		if (row >= 0 && row < nbrOfRows_+4 && column >= 0 && column < nbrOfColumns_) {
			return gameboard_[row * nbrOfColumns_ + column];
		}
		return BlockType::EMPTY;
	}

	// Returns true if block is outside or on an allready occupied square on the board.
	// Else it returns false.
	bool collision(const Block& block) const;

protected:
	// Set all squares on the board to empty.
	// Game over is set to false.
	void clearBoard();

	void setNextBlock(BlockType next);
	void setCurrentBlock(BlockType current);

	inline BlockType& blockType(int row, int column) {
		return gameboard_[row * nbrOfColumns_ + column];
	}

	Block createBlock(BlockType blockType) const;

private:
	virtual void triggerEvent(GameEvent gameEvent) {
	}

	virtual std::vector<BlockType> addExternalRows() {
		return std::vector<BlockType>();
	}

	// Adds a block directly to the board.
	// I.e. Block becomes a part of the board.
	void addBlockToBoard(const Block& block);

    int removeFilledRows(const Block& block);
    void moveRowsOneStepDown(int rowToRemove);
    
	bool isGameOver_;					// True when game is over, else false.
	Block next_;						// Next block for the player to control.
	Block current_;						// The current block for the player to control.
	std::vector<BlockType> gameboard_;	// Containing all non moving squares on the board.
	int nbrOfRows_, nbrOfColumns_;		// The size of the gameboard.
};

#endif // RAWTETRISBOARD_H
