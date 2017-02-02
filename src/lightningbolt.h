#ifndef LIGHTNINGBOLT_H
#define LIGHTNINGBOLT_H

#include "mat44.h"
#include "tetrisentry.h"
#include "lightningvertexdata.h"
#include "random.h"

#include <mw/vec2.h>
#include <mw/vertexdata.h>

#include <memory>

inline Vec2 operator*(const Mat44& m, Vec2 p) {
	return Vec2(p.x_ * m(0, 0) + p.y_ * m(0, 1) + m(0, 3), p.x_ * m(1, 0) + p.y_ * m(1, 1) + m(1, 3));
}

class LightningBolt;
using LightningBoltPtr = std::shared_ptr<LightningBolt>;

class LightningBolt : public LightningVertexData {
public:
	LightningBolt(const LightningShader& lightShader, mw::Sprite halfCircle, mw::Sprite  lineSegment, Color tInt, float alphaMultiplier, float fadeOutRate, float thickness);
	LightningBolt(const LightningShader& lightShader, mw::Sprite halfCircle, mw::Sprite  lineSegment);

	void draw(float deltaTime);

	void setOpenGlStates();

	void drawWithoutOpenGlStates(float deltaTime);
	
	void restart(Vec2 source, Vec2 dest);

	void restart();

	bool isAlive() const {
		return alpha_ >= 0;
	}
	
private:
	class Line {
	public:
		Vec2 start_, end_;

		inline Line() {
		}

		inline Line(Vec2 start, Vec2 end, float thickness) : start_(start), end_(end) {
		}
	};

	std::list<Line> createBolt(Vec2 source, Vec2 dest) const;

	// Returns the point where the bolt is at a given fraction of the way through the bolt. Passing
	// zero will return the start of the bolt, and passing 1 will return the end.
	Vec2 getPoint(float position);

	Vec2 lerp(Vec2 p1, Vec2 p2, float value);

	Line findSegmentLargerThan(float position, Vec2 dir) const;

	void addLine(Vec2 start, Vec2 end, const Color& color);

	mw::Sprite halfCircle_, lineSegment_;
	mw::VertexDataPtr vd_;

	std::list<Line> segments_;

	Color Tint_;
	Random random_;
	float alphaMultiplier_;
	float alpha_;
	float fadeOutRate_;
	float thickness_;
};

#endif // LIGHTNINGBOLT_H
