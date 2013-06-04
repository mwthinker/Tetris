#ifndef PLAYER_H
#define PLAYER_H

#include "playerinfo.h"

#include <string>
#include <queue>
#include <memory>

class Player : public PlayerInfo {
public:
	Player(int id, int width, int height, int maxLevel, bool remote);
    virtual ~Player();

	// Restart the player.
	void restart();

	bool updateBoard(TetrisBoard::Move& move, BlockType& next);
	void update(TetrisBoard::Move move);
	void update(BlockType current, BlockType next);
	void update(TetrisBoard::Move move, BlockType next);
	void update(const std::vector<BlockType>& blockTypes);

protected:
    // Pushed to a queue.
    void pushMove(TetrisBoard::Move move);

private:
	// Remove from the queue.
    bool pollMove(TetrisBoard::Move& move);

	std::queue<TetrisBoard::Move> moves_;
};

typedef std::shared_ptr<Player> PlayerPtr;

#endif // PLAYER_H
