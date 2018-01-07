#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"
#include "protocol.h"
#include "tetrisboard.h"

#include <net/connection.h>
#include <net/packet.h>

class RemotePlayer : public Player {
public:
	RemotePlayer(int id, int width, int height, bool ai, BlockType current, BlockType next);

	// @Player
	void update(double deltaTime) override {
	}

	void receive(net::Packet& packet);

	void resizeBoard(int width, int height);

	void restart(BlockType current, BlockType next);

	void setName(std::string name) {
		name_ = name;
	}

	void setLevel(int level) {
		level_ = level;
	}

	void setPoints(int points) {
		points_ = points;
	}
	
	// @Player
	bool isAi() const override {
		return ai_;
	}

private:
	bool ai_;
};

#endif // REMOTEPLAYER_H
