#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "device.h"
#include "rawtetrisboard.h"

#include <net/packet.h>

#include <string>

// Defines the packet content.
// Value of the first byte.
enum class PacketType {
	MOVE,       // Tetrisboard updates.
	STARTGAME,   // The server starts the game. All user starts the game.
	READY,       // The client is ready/unready to start.
	SERVERINFO,  // Sent from the server. The info about players and tetrisboard conditions (e.g. length and width).
	TETRIS,      // Player add rows.
	CLIENTINFO,  // A client send info to the server.
	STARTBLOCK,  // Send the start block, i.e. current block and the next block.
	PAUSE        // Pause/Unpause the game for all users.
};

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

#endif // PROTOCOL_H
