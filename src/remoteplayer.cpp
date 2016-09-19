#include "remoteplayer.h"

#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

RemotePlayer::RemotePlayer(int id, int width, int height, BlockType current, BlockType next) :
	Player(id, width, height, current, next) {

}

void RemotePlayer::receive(net::Packet& packet) {
	packet.reset();
	PacketType type;
	packet >> type;
	int id;
	packet >> id;
	int playerId;
	packet >> playerId;
	switch (type) {
		case PacketType::PLAYER_MOVE:
		{
			Move move;
			packet >> move;
			BlockType next;
			packet >> next;
			tetrisBoard_.update(move);
			tetrisBoard_.updateNextBlock(next);
			break;
		}
		case PacketType::PLAYER_TETRIS:
		{
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

void RemotePlayer::resizeBoard(int width, int height) {
	tetrisBoard_.updateRestart(height, width, tetrisBoard_.getBlockType(), tetrisBoard_.getNextBlockType());
	level_ = 1;
	points_ = 0;
}

void RemotePlayer::restart(BlockType current, BlockType next) {
	tetrisBoard_.restart(current, next);
	level_ = 1;
	points_ = 0;
}
