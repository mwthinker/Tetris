#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"
#include "tetrisboard.h"
#include "actionhandler.h"
#include "device.h"

#include "protocol.h"

#include <net/connection.h>
#include <net/packet.h>

#include <memory>
#include <queue>

class LocalPlayer : public Player {
public:
	LocalPlayer(int id, int width, int height, const DevicePtr& device, PacketSender& sender);

    void update(double deltaTime) override;

	inline const DevicePtr& getDevice() const {
		return device_;
	}

	void restart(BlockType current, BlockType next);

private:
	double calculateDownSpeed(int level) const;

	void update(Move move);

	void boardListener(GameEvent, const TetrisBoard&);

    // Controls how the moving block is moved.
	ActionHandler gravityMove_, downHandler_, leftHandler_, rightHandler_, rotateHandler_;
	int nbrOfUpdates_;
    DevicePtr device_;
	PacketSender& sender_;
};

#endif // LOCALPLAYER_H
