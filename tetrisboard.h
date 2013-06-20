#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include "block.h"

#include <vector>
#include <queue>

enum GameEvent {
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

// Represents a tetris board.
class TetrisBoard {
public:
	// Defines all valid ways of controlling the falling block.
    enum Move {ROTATE_LEFT, ROTATE_RIGHT, DOWN_GRAVITY, DOWN, LEFT, RIGHT};

    TetrisBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next);

	void restart(BlockType current, BlockType next);

	// Add a move to the falling block.
    void update(Move move);

	// Add next block.
	void add(BlockType next);

	// Triggers a gameover event to be added to the event queue.
	void triggerGameOverEvent();
    
	// Return the number of rows.
	int getNbrOfRows() const;
    
	// Return the number of columns.
	int getNbrOfColumns() const;
	
	// Return true if the game is over else false.
    bool isGameOver() const;

	// Returns all non moving squares on the board.
    const std::vector<BlockType>& getGameBoard() const;

	// Return the moving block.
    Block currentBlock() const;

	// Return the next moving block.
    Block nextBlock() const;

	// Returns true as long as there are more game events to be poll to parameter (gameEvent).
    bool pollGameEvent(GameEvent& gameEvent);

	// Add rows to be added at the bottom of the board at the next change of the moving block.
	void addRows(const std::vector<BlockType>& blockTypes);

	BlockType getBlockFromBoard(int row, int column) const;

private:
    void addExternalRows();
	Block createBlock(BlockType blockType) const;
    bool collision(Block block) const;

	BlockType& getBlockFromBoard(int row, int column);

    int removeFilledRows(const Block& block);
    void moveRowsOneStepDown(int rowToRemove);

    Block next_;						// Next block for the player to control.
	Block current_;						// The current block for the player to control.
	
	std::vector<BlockType> gameboard_;	// Containing all non moving squares on the board.
	std::vector<int> rowsFilled_;		// Index represents the row and the value represents the number of pieces on the row.

	std::vector<BlockType> squaresToAdd_;
	std::queue<GameEvent> gameEvents_;

    int nbrOfRows_, nbrOfColumns_;		// The size of the gameboard.
    bool isGameOver_;					// True when game is over, else false.

	std::queue<BlockType> nextBlockQueue_;
};

#endif // TETRISBOARD_H
