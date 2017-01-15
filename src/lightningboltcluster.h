#ifndef LIGHTNINGBOLTCLUSTER_H
#define LIGHTNINGBOLTCLUSTER_H

#include "lightningbolt.h"
#include "tetrisentry.h"

#include <vector>

class LightningBoltCluster {
public:
	LightningBoltCluster(TetrisEntry spriteEntry, Vec2 source, Vec2 dest) : generator_(std::random_device()()) {
		mw::Sprite sprite = spriteEntry.getChildEntry("halfCircle").getSprite();
		mw::Sprite sprite2 = spriteEntry.getChildEntry("lineSegment").getSprite();		
	}

	void restart(const std::vector<Vec2>& points, int lightBoltsNbr, int randomNbr) {
		if (points.size() > 0) {
			points_ = points;
			std::vector<int> indexes(points.size(), 0);
			for (unsigned int i = 0; i < indexes.size(); ++i) {
				indexes[i] = i;
			}
		}
	}

	void draw(float deltaTime) {

	}

private:
	// Return the closest end point of a random selection of points.
	// Vec2 point - point the end point is compared to.
	// int number - the number randomly choosen points to select the closest end point from.
	Vec2 calculateEndPoint(Vec2 point, int number) const {
		Vec2 endPoint = Vec2(100000, 100000); // Point far far away.
		for (int i = 0; i < number; ++i) {
			Vec2 tmp = points_[rand(0, points_.size() - 1)];
			// Is the new point closer?
			if ((point - tmp).magnitudeSquared() < (point - endPoint).magnitudeSquared()) {
				endPoint = tmp;
			}
		}
		return endPoint;
	}

	int rand(unsigned int min, unsigned int max) const {
		std::uniform_int_distribution<> dist(min, max);
		//return dist(generator_);
		return 0;
	}

	std::default_random_engine generator_;
	std::vector<Vec2> points_;

};

#endif // LIGHTNINGBOLTCLUSTER_H
