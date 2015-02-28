#include "protocol.h"

#include <sstream>

net::Packet& operator<<(net::Packet& packe1t, const net::Packet& packet2) {
	int size = packet2.getSize();
	for (int i = 1; i < size; ++i) {
		packe1t << packet2[i];
	}
	return packe1t;
}

net::Packet& operator<<(net::Packet& packet, Input input) {
	char data = input.rotate_;
	data <<= 1;
	data += input.down_;
	data <<= 1;
	data += input.left_;
	data <<= 1;
	data += input.right_;
	packet << data;
	return packet;
}

net::Packet& operator>>(net::Packet& packet, Input& input) {
	char data;
	packet >> data;
	char bit = 1;
	input.right_ = (bit & data) > 0;
	bit <<= 1;
	input.left_ = (bit & data) > 0;
	bit <<= 1;
	input.down_ = (bit & data) > 0;
	bit <<= 1;
	input.rotate_ = (bit & data) > 0;
	return packet;
}


net::Packet& operator<<(net::Packet& packet, PacketType type) {
	packet << (char) type;
	return packet;
}

net::Packet& operator>>(net::Packet& packet, PacketType& type) {
	char tmp;
	packet >> tmp;
	type = (PacketType) tmp;
	return packet;
}


net::Packet& operator<<(net::Packet& packet, Move move) {
	packet << (char) move;
	return packet;
}

net::Packet& operator>>(net::Packet& packet, Move& move) {
	char tmp;
	packet >> tmp;
	move = (Move) tmp;
	return packet;
}


net::Packet& operator<<(net::Packet& packet, BlockType type) {
	packet << (char) type;
	return packet;
}

net::Packet& operator>>(net::Packet& packet, BlockType& type) {
	char tmp;
	packet >> tmp;
	type = (BlockType) (tmp);
	return packet;
}


net::Packet& operator<<(net::Packet& packet, int number) {
	packet << (char) number;
	return packet;
}

net::Packet& operator>>(net::Packet& packet, int& number) {
	char tmp;
	packet >> tmp;
	number = (int) (tmp);
	return packet;
}


net::Packet& operator<<(net::Packet& packet, bool data) {
	packet << (char) data;
	return packet;
}

net::Packet& operator>>(net::Packet& packet, bool& data) {
	char tmp;
	packet >> tmp;
	data = tmp > 0;
	return packet;
}


net::Packet& operator<<(net::Packet& packet, const std::string& text) {
	packet << (int) text.size();
	for (char c : text) {
		packet << c;
	}
	return packet;
}

net::Packet& operator>>(net::Packet& packet, std::string& text) {
	int size;
	packet >> size;
	for (int i = 0; i < size; ++i) {
		char c;
		packet >> c;
		text.push_back(c);
	}
	return packet;
}
