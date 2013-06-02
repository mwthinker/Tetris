#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "actionhandler.h"
#include "movehandler.h"
#include "device.h"
#include "tetrisboard.h"
#include "player.h"

#include <memory>

class LocalPlayer : public Player {
public:
    LocalPlayer(int id, int width, int height, int maxLevel, const DevicePtr& device);
    ~LocalPlayer();

    void update(double deltaTime);

private:
	double calculateDownSpeed(int level) const;

    double time_; // Gametime
	double lastDownTime_; // The gametime for the last "gravity" move.

    // Controls how the moving block is moved
    ActionHandler* leftHandler_;
    ActionHandler* rightHandler_;
    MoveHandler* downHandler_;
    ActionHandler* rotateHandler_;

    DevicePtr device_;
};

typedef std::shared_ptr<LocalPlayer> LocalPlayerPtr;

#endif // LOCALPLAYER_H
