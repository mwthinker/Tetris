#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "player.h"
#include "tetrisboard.h"
#include "actionhandler.h"
#include "device.h"

#include <memory>

class LocalPlayer;
typedef std::shared_ptr<LocalPlayer> LocalPlayerPtr;

class LocalPlayer : public Player {
public:
    LocalPlayer(int id, int width, int height, int maxLevel, const DevicePtr& device);

    void update(double deltaTime);
	void updateAi() override;

private:
	double calculateDownSpeed(int level) const;
	void polledGameEvent(GameEvent gameEvent);

	ActionHandler gravityMove_;

    // Controls how the moving block is moved.
	ActionHandler downHandler_, leftHandler_, rightHandler_, rotateHandler_;

    DevicePtr device_;
	double fastestDownTime_;
};

#endif // LOCALPLAYER_H
