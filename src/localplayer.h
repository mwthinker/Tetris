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
	LocalPlayer(int id, int width, int height, BlockType moving, BlockType next, const DevicePtr& device, PacketSender& sender);

    void update(double deltaTime) override;

	inline const DevicePtr& getDevice() const {
		return device_;
	}

	void restart(BlockType current, BlockType next);

	inline int getLevelUpCounter() const {
		return levelUpCounter_;
	}

	inline int getClearedRows() const {
		return clearedRows_;
	}

	inline void setLevelUpCounter(int levelUpCounter) {
		levelUpCounter_ = levelUpCounter;
	}

	inline void setClearedRows(int clearedRows) {
		clearedRows_ = clearedRows;
	}

	inline void setLevel(int level) {
		level_ = level;
	}

	inline void setPoints(int points) {
		points_ = points;
	}

private:
	double calculateDownSpeed(int level) const;

	void update(Move move);

	void boardListener(GameEvent, const TetrisBoard&);

    // Controls how the moving block is moved.
	ActionHandler gravityMove_, downHandler_, leftHandler_, rightHandler_, rotateHandler_;
	int nbrOfUpdates_;
    DevicePtr device_;
	PacketSender& sender_;
	int levelUpCounter_;
	int clearedRows_;
};

#endif // LOCALPLAYER_H
