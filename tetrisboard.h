#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include "square.h"
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

typedef std::vector<Square> Squares;

// Represent a tetris board.
class TetrisBoard {
public:
	// Defines all valid ways of controlling the falling block.
    enum Move {ROTATE_LEFT, ROTATE_RIGHT, DOWN_GRAVITY, DOWN, LEFT, RIGHT};

    TetrisBoard(int nbrOfRows = 20, int nbrOfColumns = 10, double newLineSquaresPercenters = 0.8);

	// Add a move to the falling block.
    void update(Move move);

	// Triggers a gameover event to be added to the event queue.
	void triggerGameOverEvent();
    
	// Return the number of rows.
	int getNbrOfRows() const;
    
	// Return the number of columns.
	int getNbrOfColumns() const;
	
	// Return true if the game is over else false.
    bool isGameOver() const;

	// Returns all non moving squares on the board.
    const Squares& getGameBoard() const;

	// Return the moving block.
    Block currentBlock() const;

	// Return the next moving block.
    Block nextBlock() const;

	void setNonRandomCurrentBlockType(BlockType blockType);
	void setNonRandomNextBlockType(BlockType blockType);
	BlockType getNonRandomCurrentBlockType() const;
	BlockType getNonRandomNextBlockType() const;
	void setDecideRandomBlockType(bool decideRandom);
	bool isDecideRandomBlockType() const;

	// Returns true as long as there are more game events to be poll to parameter (gameEvent).
    bool pollGameEvent(GameEvent& gameEvent);

	// Add rows to be added at the bottom of the board at the next change of the moving block.
	void addRows(const std::vector<BlockType>& blockTypes);
	
	// Generate a vector 
	std::vector<BlockType> generateRow() const;
private:
	void init(int nbrOfRows, int nbrOfColumns, double newLineSquaresPercenters);
    void addRow();
    void fillRow(const Square& square);
    BlockType generateBlockType() const;
    Block generateBlock() const;
    bool collision(Block block) const;

    int removeFilledRows();
    void removeSquares(int row);
    void moveRowsOneStepDown(int highestRow);

    Block next_;						// Next Block for the player to control.
	Block current_;						// The current block for the player to control.

	double squaresPerLength_;           // Added row is filled by squaresPerLength_% of new squares.
	Squares gameboard_;		            // Containing all non moving squares on the board

    int addRows_;
	std::vector<BlockType> squaresToAdd_;
	std::queue<GameEvent> gameEvents_;

    int nbrOfRows_, nbrOfColumns_;		// The size of the gameboard.
    bool isGameOver_;                   // True when game is over, else false.

	BlockType nonRandomCurrentBlock_;
	BlockType nonRandomNextBlock_;
	bool decideRandom_;
};

#endif // TETRISBOARD_H
