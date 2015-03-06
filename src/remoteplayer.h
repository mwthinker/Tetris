#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"
#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

class RemotePlayer : public Player {
public:
	RemotePlayer(int id, int width, int height, BlockType moving, BlockType next) : Player(id, width, height, moving, next) {
    }

    void update(double deltaTime) override {
    }

	void receive(net::Packet& packet) {
		PacketType type;
		packet >> type;
		switch (type) {
			case PacketType::PLAYER_MOVE: {
				Move move;
				packet >> move;
				BlockType next;
				packet >> next;
				tetrisBoard_.update(move);
				break;
			}
			case PacketType::PLAYER_TETRIS: {
				std::vector<BlockType> blockTypes;
				while (packet.dataLeftToRead() > 0) {
					BlockType type;
					packet >> type;
				}
				tetrisBoard_.addRows(blockTypes);
				break;
			}
			case PacketType::PLAYER_START_BLOCK: {
				BlockType current;
				packet >> current;
				BlockType next;
				packet >> next;
				tetrisBoard_.restart(current, next);
				break;
			}
			case PacketType::PLAYER_NAME:
				packet >> name_;
				break;
			case PacketType::PLAYER_LEVEL:
				packet >> level_;
				break;
			case PacketType::PLAYER_POINTS:
				packet >> points_;
				break;
		}
	}

	void setName(std::string name) {
		name_ = name;
	}

	void setLevel(int level) {
		level_ = level;
	}

	void setPoints(int points) {
		points_ = points;
	}
};

#endif // REMOTEPLAYER_H
