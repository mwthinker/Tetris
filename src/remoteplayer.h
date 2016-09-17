#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"
#include "protocol.h"
#include "tetrisboard.h"

#include <net/connection.h>
#include <net/packet.h>

class RemotePlayer : public Player {
public:
	RemotePlayer(int id, int width, int height, BlockType current, BlockType next);

	// @Player
	inline void update(double deltaTime) override {
	}

	void receive(net::Packet& packet);

	void resizeBoard(int width, int height);

	void restart(BlockType current, BlockType next);

	inline void setName(std::string name) {
		name_ = name;
	}

	inline void setLevel(int level) {
		level_ = level;
	}

	inline void setPoints(int points) {
		points_ = points;
	}
};

#endif // REMOTEPLAYER_H
