#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"

class RemotePlayer : public Player {
public:
    RemotePlayer(int id, int width, int height) : Player(id, width, height, true, false) {
    }

    void update(double deltaTime, int level) {
    }
};

#endif // REMOTEPLAYER_H
