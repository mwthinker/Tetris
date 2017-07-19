#ifndef GAMEBOARDCOMPONENT_H
#define GAMEBOARDCOMPONENT_H

#include "drawblock.h"
#include "drawrow.h"
#include "boardvertexdata.h"
#include "tetrisboard.h"
#include "boardshader.h"
#include "drawblock.h"
#include "mat44.h"
#include "lightningvertexdata.h"
#include "lightningbolt.h"
#include "lightningboltcluster.h"
#include "lightningshader.h"

#include <gui/component.h>

#include <list>

class GameboardComponent : public gui::Component {
public:
	GameboardComponent();
	~GameboardComponent();

protected:
	// @gui::Component
	void draw(const gui::Graphic& graphic, double deltaTime) override;

private:
	void addDrawRowAtTheTop(const TetrisBoard& tetrisBoard, int nbr);

	void initStaticBackground(const LightningShader& lightningShader, const BoardShader& boardShader, Player& player);

	std::list<DrawRowPtr> rows_;
	std::list<DrawRowPtr> freeRows_;
	BoardVertexDataPtr staticVertexData_;
	DrawBlockPtr currentBlockPtr_, nextBlockPtr_;
};

#endif // GAMEBOARDCOMPONENT_H
