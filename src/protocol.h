#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "device.h"
#include "rawtetrisboard.h"

#include <net/packet.h>

#include <string>

// Defines the packet content.
// Value of the first byte.
enum class PacketType {
	START,                 // The server ends the looby and starts the game.
	PAUSE,                 // Pause/Unpause the game.
	CONNECTION_RESTART,    // The game is restarted.
	CONNECTION_BOARD_SIZE, // The board size, restarts the game.
	CONNECTION_INFO,       // The info about players and tetrisboard conditions (e.g. length and width) for one connection.
	CONNECTION_ID,         // The id for the connection receiving this packet.
	CONNECTION_DISCONNECT, // The connection with the id was disconnected.
	PLAYER_MOVE,           // A move to update the tetrisboard.
	PLAYER_TETRIS,           // A move to update the tetrisboard.
	PLAYER_START_BLOCK,    // The start block for a player, i.e. current block and the next block.
	PLAYER_NAME,           // The name for a player.
	PLAYER_LEVEL,          // The level for a player.
	PLAYER_POINTS       // The point for a player.
};

static const int SERVER_CONNECTION_ID = 0;

net::Packet& operator<<(net::Packet& packe1t, const net::Packet& packet2);

net::Packet& operator<<(net::Packet& packet, Input input);

net::Packet& operator>>(net::Packet& packet, Input& input);


net::Packet& operator<<(net::Packet& packet, PacketType type);

net::Packet& operator>>(net::Packet& packet, PacketType& type);


net::Packet& operator<<(net::Packet& packet, Move move);

net::Packet& operator>>(net::Packet& packet, Move& move);


net::Packet& operator<<(net::Packet& packet, BlockType type);

net::Packet& operator>>(net::Packet& packet, BlockType& type);


net::Packet& operator<<(net::Packet& packet, int number);

net::Packet& operator>>(net::Packet& packet, int& number);


net::Packet& operator<<(net::Packet& packet, bool data);

net::Packet& operator>>(net::Packet& packet, bool& data);


net::Packet& operator<<(net::Packet& packet, const std::string& text);

net::Packet& operator>>(net::Packet& packet, std::string& text);

class PacketSender {
public:
	virtual void sendToAll(const net::Packet& packet) const = 0;

	bool active_;

protected:
	PacketSender() : active_(false) {
	}

	~PacketSender() {
	}

};

#endif // PROTOCOL_H
