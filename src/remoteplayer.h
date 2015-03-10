#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"
#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

class RemotePlayer : public Player {
public:
	RemotePlayer(int id, int width, int height, BlockType current, BlockType next) :
		Player(id, width, height, current, next) {

	}

	void update(double deltaTime) override {
	}

	void receive(net::Packet& packet) {
		packet.reset();
		PacketType type;
		packet >> type;
		int id;
		packet >> id;
		int playerId;
		packet >> playerId;
		switch (type) {
			case PacketType::PLAYER_MOVE: {
				Move move;
				packet >> move;
				BlockType next;
				packet >> next;
				tetrisBoard_.update(move);
				tetrisBoard_.updateNextBlock(next);
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

	void resizeBoard(int width, int height) {
		tetrisBoard_.updateRestart(height, width, tetrisBoard_.getBlockType(), tetrisBoard_.getNextBlockType());
		clearedRows_ = 0;
		level_ = 1;
		points_ = 0;
	}

	void restart(BlockType current, BlockType next) {
		tetrisBoard_.restart(current, next);
		clearedRows_ = 0;
		level_ = 1;
		points_ = 0;
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
