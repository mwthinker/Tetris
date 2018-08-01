#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"
#include "tetrisboard.h"
#include "actionhandler.h"
#include "device.h"
#include "playerdata.h"

#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

class LocalPlayer : public Player {
public:
	LocalPlayer(int connectionId, int playerId, int width, int height,
		BlockType moving, BlockType next, const IDevicePtr& device, PacketSender& sender);

	LocalPlayer(int connectionId, int playerId, int width, int height,
		const PlayerData& playerData, PacketSender& sender);

	// @Player
    void update(double deltaTime) override;

	const IDevicePtr& getDevice() const {
		return device_;
	}

	void restart(BlockType current, BlockType next);

	void restart(int width, int height);

	int getLevelUpCounter() const {
		return levelUpCounter_;
	}

	void setLevelUpCounter(int levelUpCounter) {
		levelUpCounter_ = levelUpCounter;
	}

	void setLevel(int level) {
		level_ = level;
	}

	void setPoints(int points) {
		points_ = points;
	}

	void setConnectionId(int connectionId) {
		connectionId_ = connectionId;
	}

	void endGame();

	void addExternalRows(const std::vector<BlockType>& blockTypes);

	bool isAi() const override {
		return device_->isAi();
	}
	
private:
	void update(Move move);

	void boardListener(GameEvent, const TetrisBoard&);

    // Objects controlling how the moving block is moved.
	ActionHandler gravityMove_, downHandler_, leftHandler_, rightHandler_, rotateHandler_, downGroundHandler_;
	IDevicePtr device_;
	PacketSender& sender_;
	int levelUpCounter_;
	int connectionId_;
};

#endif // LOCALPLAYER_H
