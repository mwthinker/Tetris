#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"
#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

class RemotePlayer : public Player {
public:
    RemotePlayer(int id, int width, int height) : Player(id, width, height) {
    }

    void update(double deltaTime) override {
    }

	void receive(net::Packet& packet) {
		PacketType type;
		packet >> type;
		switch (type) {
			case PacketType::MOVE: {
				Move move;
				BlockType next;
				tetrisBoard_.update(move);
				packet >> move;
				packet >> next;
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
};

#endif // REMOTEPLAYER_H
