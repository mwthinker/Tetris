#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"

class RemotePlayer : public Player {
public:
    RemotePlayer(int id, int width, int height, int maxLevel) : Player(id,width,height,maxLevel,true) {

    }

    void update(double deltaTime) {
    }
};

#endif // REMOTEPLAYER_H
