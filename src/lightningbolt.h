#ifndef LIGHTNINGBOLT_H
#define LIGHTNINGBOLT_H

#include "mat44.h"
#include "vertexdata.h"
#include "tetrisentry.h"
#include "lightningvertexdata.h"

#include <mw/vec2.h>

#include <memory>
#include <random>

inline Vec2 operator*(const Mat44& m, Vec2 p) {
	return Vec2(p.x_ * m(0, 0) + p.y_ * m(0, 1) + m(0, 3), p.x_ * m(1, 0) + p.y_ * m(1, 1) + m(1, 3));
}

class LightningBolt;
using LightningBoltPtr = std::shared_ptr<LightningBolt>;

class LightningBolt : public LightningVertexData {
public:
	LightningBolt(const LightningShader& lightShader, TetrisEntry spriteEntry, Vec2 source, Vec2 dest);

	LightningBolt(const LightningShader& lightShader, TetrisEntry spriteEntry, Vec2 source, Vec2 dest, Color color);

	void draw(float deltaTime);
private:
	class Line {
	public:
		Vec2 start_, end_;

		inline Line() {
		}

		inline Line(Vec2 start, Vec2 end, float thickness) : start_(start), end_(end) {
		}
	};

	std::list<Line> createBolt(Vec2 source, Vec2 dest);

	// Returns the point where the bolt is at a given fraction of the way through the bolt. Passing
	// zero will return the start of the bolt, and passing 1 will return the end.
	Vec2 getPoint(float position);

	Vec2 lerp(Vec2 p1, Vec2 p2, float value);

	float rand(float min, float max);

	Line findSegmentLargerThan(float position, Vec2 dir) const;

	void addLine(Vec2 start, Vec2 end, const Color& color);

	void addSquare(const mw::Sprite& sprite, const Color& color, Vec2 position, float angle, Vec2 scale);

	mw::Sprite halfCircle_, lineSegment_;
	VertexDataPtr vd_;

	std::random_device rd_;
	std::default_random_engine generator_;

	std::list<Line> segments_;

	Color Tint_;
	float alphaMultiplier_;
	float alpha_;
	float fadeOutRate_;
	float thickness_;
};

#endif // LIGHTNINGBOLT_H
