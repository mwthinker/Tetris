#include "localconnection.h"

const double LocalConnection::timeStep_ = 1.0 / 60;

LocalConnection::LocalConnection(PacketSender& packetSender) :
	packetSender_(packetSender),
	id_(UNDEFINED_CONNECTION_ID) {
}

void LocalConnection::setPlayers(int width, int height, const std::vector<DevicePtr>& devices) {
	players_.clear();
	for (const auto& device : devices) {
		auto player = std::make_shared<LocalPlayer>(id_, players_.size(), width, height,
			randomBlockType(), randomBlockType(), device, packetSender_);
		players_.push_back(player);
	}

	if (packetSender_.isActive()) {
		packetSender_.sendToAll(getClientInfo());
	}
}

void LocalConnection::addPlayer(int width, int height, const PlayerData& playerData) {
	auto player = std::make_shared<LocalPlayer>(id_, players_.size(), width, height,
		playerData, packetSender_);
	players_.push_back(player);
}

void LocalConnection::restart() {
	for (auto& player : players_) {
		player->restart(randomBlockType(),
			randomBlockType());
	}

	if (packetSender_.isActive()) {
		sendConnectionStartBlock();
	}
}

void LocalConnection::restart(int width, int height) {
	for (auto& player : players_) {
		player->restart(width, height);
	}

	if (packetSender_.isActive()) {
		sendConnectionStartBlock();
	}
}

int LocalConnection::getNbrAiPlayers() const {
	int nbr = 0;
	for (auto& player : players_) {
		if (player->isAi()) {
			++nbr;
		}
	}

	return nbr;
}

void LocalConnection::updateGame(double deltaTime) {
	// DeltaTime to big?
	if (deltaTime > 0.250) {
		// To avoid spiral of death.
		deltaTime = 0.250;
	}
	// Update using fixed time step.
	while (deltaTime >= timeStep_) {
		deltaTime -= timeStep_;
		for (auto& player : players_) {
			player->update(timeStep_);
		}
	}
	// Update the remaining time.
	for (auto& player : players_) {
		player->update(deltaTime);
	}
}

net::Packet LocalConnection::getClientInfo() const {
	net::Packet packet;
	packet << PacketType::CONNECTION_INFO;
	packet << id_;
	for (auto& player : players_) {
		packet << player->getName();
		packet << player->getLevel();
		packet << player->getPoints();
		packet << player->isAi();

		auto& board = player->getTetrisBoard();
		packet << board.getBlockType();
		packet << board.getNextBlockType();
	}
	return packet;
}

void LocalConnection::setId(int id) {
	id_ = id;
	for (auto& player : players_) {
		player->setConnectionId(id_);
	}
}

void LocalConnection::sendConnectionStartBlock() {
	net::Packet packet;
	packet << PacketType::CONNECTION_START_BLOCK;
	packet << id_;
	for (auto& player : players_) {
		packet << player->getTetrisBoard().getBlockType();
		packet << player->getTetrisBoard().getNextBlockType();
	}
	LocalConnection::packetSender_.sendToAll(packet);
}
