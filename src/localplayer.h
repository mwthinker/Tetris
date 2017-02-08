#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"
#include "tetrisboard.h"
#include "actionhandler.h"
#include "device.h"

#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

class LocalPlayer : public Player {
public:
	LocalPlayer(int connectionId, int playerId, int width, int height,
		BlockType moving, BlockType next, const DevicePtr& device, PacketSender& sender);

	LocalPlayer(int connectionId, int playerId, int width, int height, const std::vector<BlockType>& board, int levelUpCounter, int points, int level,
		Block current, BlockType next, const DevicePtr& device, PacketSender& sender);

	// @Player
    void update(double deltaTime) override;

	inline const DevicePtr& getDevice() const {
		return device_;
	}

	void restart(BlockType current, BlockType next);

	void resizeBoard(int width, int height);

	inline int getLevelUpCounter() const {
		return levelUpCounter_;
	}

	inline void setLevelUpCounter(int levelUpCounter) {
		levelUpCounter_ = levelUpCounter;
	}

	inline void setLevel(int level) {
		level_ = level;
	}

	inline void setConnectionId(int connectionId) {
		connectionId_ = connectionId;
	}

	void endGame();

	void addExternalRows(const std::vector<BlockType>& blockTypes);
	
private:
	void update(Move move);

	void boardListener(GameEvent, const TetrisBoard&);

    // Objects controling how the moving block is moved.
	ActionHandler gravityMove_, downHandler_, leftHandler_, rightHandler_, rotateHandler_, downGroundHandler_;
	int nbrOfUpdates_;
    DevicePtr device_;
	PacketSender& sender_;
	int levelUpCounter_;
	int connectionId_;
	float watingTime_;
};

#endif // LOCALPLAYER_H
