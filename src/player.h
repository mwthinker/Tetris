#ifndef PLAYER_H
#define PLAYER_H

#include "playerinfo.h"

#include <string>
#include <queue>
#include <memory>

class Player : public PlayerInfo {
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

protected:
	// Pushed to a queue.
	void pushMove(Move move);

private:
	// Remove from the queue.
	bool pollMove(Move& move);
	std::queue<Move> moves_;

	double squaresPerLength_;

	const bool ai_;
	const int id_;
};

typedef std::shared_ptr<Player> PlayerPtr;

#endif // PLAYER_H
