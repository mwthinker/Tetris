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
			case PacketType::SERVERINFO: {
				int width, height;
				packet >> width >> height;
				tetrisBoard_.updateRestart(width, height,
					tetrisBoard_.getBlockType(), tetrisBoard_.getNextBlockType());
				break;
			}
			case PacketType::MOVE: {
				Move move;
				packet >> move;
				BlockType next;
				packet >> next;
				tetrisBoard_.update(move);
				break;
			}
			case PacketType::TETRIS: {
				std::vector<BlockType> blockTypes;
				while (packet.dataLeftToRead() > 0) {
					BlockType type;
					packet >> type;
				}
				tetrisBoard_.addRows(blockTypes);
				break;
			}
			case PacketType::STARTBLOCK: {
				BlockType current;
				packet >> current;
				BlockType next;
				packet >> next;
				tetrisBoard_.restart(current, next);
				break;
			}
			case PacketType::PLAYERNAME: {
				packet >> name_;
				break;
			}
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
