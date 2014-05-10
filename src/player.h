#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"

#include <string>
#include <queue>
#include <memory>

class Player;
typedef std::shared_ptr<Player> PlayerPtr;

class Player {
public:
	Player(int id, int width, int height, bool remote, bool ai);
	
	virtual ~Player() {
	}

	// Restart the player.
	void restart();

	// Is supposed to call pushMove with the appropriate move.
	// This will indirectly update the tetrisboard.
	virtual void update(double deltaTime) = 0;

	bool updateBoard(Move& move, BlockType& next);
	void update(Move move);
	void update(BlockType current, BlockType next);
	void update(Move move, BlockType next);
	void update(const std::vector<BlockType>& blockTypes);
	std::vector<BlockType> generateRow() const;

	virtual void updateAi() {
	}

	// Returns the players id.
	int getId() const;

	// Get the current block type for the board.
	BlockType getCurrentBlock() const;

	// Get the next block type for the board.
	BlockType getNextBlock() const;

	// Return true when a gameEvent is polled otherwise false.
	bool pollGameEvent(GameEvent& gameEvent);

	void setName(std::string name);
	std::string getName() const;

	void setLevel(int level);
	int getLevel() const;

	// Returns the tetrisboard.
	const TetrisBoard& getTetrisBoard() const;

protected:
	// Pushed to a queue.
	void pushMove(Move move);

private:
	// Remove from the queue.
	bool pollMove(Move& move);
	std::queue<Move> moves_;
	std::queue<GameEvent> gameEvents_;

	double squaresPerLength_;

	const bool ai_;
	const int id_;
	std::string name_;
	TetrisBoard tetrisBoard_;
	int level_;
};

#endif // PLAYER_H
