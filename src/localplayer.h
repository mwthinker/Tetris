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
    LocalPlayer(int id, int width, int height, const DevicePtr& device);

    void update(double deltaTime) override;
	void updateAi() override;

	inline const DevicePtr& getDevice() const {
		return device_;
	}
private:
	double calculateDownSpeed(int level) const;

    // Controls how the moving block is moved.
	ActionHandler gravityMove_, downHandler_, leftHandler_, rightHandler_, rotateHandler_;
	int nbrOfUpdates_;
    DevicePtr device_;
};

#endif // LOCALPLAYER_H
