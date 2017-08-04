#ifndef LIGHTNINGBOLTCLUSTER_H
#define LIGHTNINGBOLTCLUSTER_H

#include "lightningbolt.h"
#include "random.h"

#include <mw/buffer.h>

#include <random>
#include <vector>

class LightningBoltCluster {
public:
	LightningBoltCluster() {
	}

	LightningBoltCluster(const LightningShader& lightShader, const mw::Sprite& halfCircle, const mw::Sprite& lineSegment,
		const std::vector<Vec2>& points, int lightBoltsNbr, float boltRate, int maxBolts) :
		halfCircle_(halfCircle),
		lineSegment_(lineSegment),
		boltRate_(boltRate),
		maxBolts_(maxBolts),
		time_(0.0),
		nearestNbr_(1),
		nbrbolts_(0.0),
		usedBolts_(0) {

		points_ = points;
		mw::Buffer buffer(mw::Buffer::DYNAMIC);
		for (int i = 0; i < lightBoltsNbr; ++i) {
			auto bolt = std::make_shared<LightningBolt>(lightShader, halfCircle_, lineSegment_);
			deadbolts_.push_back(bolt);
			buffer.addVertexData(bolt);
		}
		buffer.uploadToGraphicCard();
	}

	void setNearestNbr(int nbrNear) {
		nearestNbr_ = nbrNear;
	}

	void setUsedbolts(int usedBolts) {
		usedBolts_ = usedBolts;
	}

	void setMaxbolts(int maxBolts) {
		maxBolts_ = maxBolts;
	}

	void setBoltRate(float boltRate) {
		boltRate_ = boltRate;
	}

	void restart() {
		time_ = 0.0;
		usedBolts_ = 0;
		nbrbolts_ = 0.0;
		deadbolts_.insert(deadbolts_.end(), bolts_.begin(), bolts_.end());
		bolts_.clear();
	}

	void restart(const std::vector<Vec2>& points) {
		time_ = 0.0;
		nbrbolts_ = 0.0;
		usedBolts_ = 0;
		deadbolts_.insert(deadbolts_.end(), bolts_.begin(), bolts_.end());
		bolts_.clear();

		if (points.size() > 0) {
			points_ = points;
		}
	}

	void draw(float deltaTime) {
		time_ += deltaTime;

		nbrbolts_ += boltRate_ * deltaTime;

		int nbr = (int) nbrbolts_;

		for (int i = 0; i < nbr && (usedBolts_ < maxBolts_ || maxBolts_ < 0); ++i) {
			if (!deadbolts_.empty()) {
				int index = random_.generateInt(0, deadbolts_.size() - 1);
				auto bolt = removeFromVector(deadbolts_, index);
				bolts_.push_back(bolt);
				Vec2 start = points_[random_.generateInt(0, points_.size() - 1)];
				Vec2 end = calculateEndPoint(start, nearestNbr_);
				bolt->restart(start, end);
				++usedBolts_;
			}
		}
		nbrbolts_ -= nbr;

		if (!bolts_.empty()) {
			bolts_.front()->setOpenGlStates();
			
			int size = bolts_.size();

			for (int i = 0; i < size; ++i) {
				auto bolt = bolts_[i];
				if (bolt->isAlive()) {
					bolt->draw(deltaTime);
				} else {
					size -= 1;
					removeFromVector(bolts_, i);
					deadbolts_.push_back(bolt);
				}
			}
		}
	}

private:
	static LightningBoltPtr removeFromVector(std::vector<LightningBoltPtr>& v, int unsigned index) {
		auto element = v[index];
		std::swap(v[index], v.back());
		v.pop_back();
		return element;
	}

	// Return the closest end point of a random selection of points.
	// Vec2 point - point the end point is compared to.
	// int number - the number randomly choosen points to select the closest end point from.
	Vec2 calculateEndPoint(Vec2 point, int number) const {
		Vec2 endPoint = Vec2(100000, 100000); // Point far far away.
		for (int i = 0; i < number; ++i) {
			Vec2 tmp = points_[random_.generateInt(0, points_.size() - 1)];
			// Is the new point closer?
			if ((point - tmp).magnitudeSquared() < (point - endPoint).magnitudeSquared()) {
				endPoint = tmp;
			}
		}
		return endPoint;
	}

	std::vector<Vec2> points_;
	std::vector<LightningBoltPtr> bolts_;
	std::vector<LightningBoltPtr> deadbolts_;
	mw::Sprite halfCircle_, lineSegment_;
	int nearestNbr_;
	double time_;
	float boltRate_;
	Random random_;

	float nbrbolts_;
	int usedBolts_;
	int maxBolts_;
};

#endif // LIGHTNINGBOLTCLUSTER_H
