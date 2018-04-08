#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "device.h"
#include "rawtetrisboard.h"

#include <net/packet.h>

#include <string>

// Defines the packet content.
// Value of the first byte.
enum class PacketType {
	PAUSE,                 // Pause/Unpause the game.
	RESTART,               // Restart the game.
	BOARD_SIZE,            // Change board size.
	CONNECTION_INFO,       // The info about players and tetrisboard conditions.
	CONNECTION_DISCONNECT, // The connection with the id was disconnected.
	CONNECTION_START_BLOCK,// The start blocks for all players for a connection.
	PLAYER_MOVE,           // A move to update the tetrisboard for a player.
	PLAYER_TETRIS,         // Add rows to a player.
	PLAYER_NAME,           // The name for a player.
	PLAYER_LEVEL           // The level for a player.
};

static const int SERVER_CONNECTION_ID = 0;
static const int UNDEFINED_CONNECTION_ID = -1;

net::Packet& operator<<(net::Packet& packet1, const net::Packet& packet2);

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

	virtual bool isActive() const = 0;
};

#endif // PROTOCOL_H
