#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"

class RemotePlayer : public Player {
public:
    RemotePlayer(int id) : Player(id) {

    }

    void update(double deltaTime) {
    }
};

#endif // REMOTEPLAYER_H
