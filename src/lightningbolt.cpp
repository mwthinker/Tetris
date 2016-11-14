#include "lightningbolt.h"

LightningBolt::LightningBolt(DynamicBuffer& buffer, TetrisEntry spriteEntry, Vec2 source, Vec2 dest) : LightningBolt(buffer, spriteEntry, source, dest, Color(1, 1, 1)) {
}

LightningBolt::LightningBolt(DynamicBuffer& buffer, TetrisEntry spriteEntry, Vec2 source, Vec2 dest, Color color) : generator_(rd_()),
	Tint_(color),
	alpha_(1.f),
	alphaMultiplier_(0.8f),
	fadeOutRate_(0.05f),
	thickness_(8) {

	halfCircle_ = spriteEntry.getChildEntry("halfCircle").getSprite();
	lineSegment_ = spriteEntry.getChildEntry("lineSegment").getSprite();
	vd_ = buffer.pollFirstFree();

	segments_ = createBolt(source, dest);
}

void LightningBolt::addLine(Vec2 start, Vec2 end, const Color& color) {
	Vec2 tangent = end - start;
	float segmentLength = tangent.magnitude();

	Vec2 leftMiddle = -thickness_ * 0.5f * tangent / segmentLength + start;
	Vec2 middle = start + tangent * 0.5f;
	Vec2 rightMiddle = thickness_ * 0.5f * tangent / segmentLength + end;

	float angle = tangent.angle();
	addSquare(halfCircle_, color, leftMiddle, 0.f + angle, Vec2(thickness_, thickness_));
	addSquare(lineSegment_, color, middle, 0.f + angle, Vec2(segmentLength, thickness_));
	addSquare(halfCircle_, color, rightMiddle, 3.1416f + angle, Vec2(thickness_, thickness_));
}

void LightningBolt::addSquare(const mw::Sprite& sprite, const Color& color, Vec2 position, float angle, Vec2 scale) {
	Mat44 m = Mat44::I;

	mw::translate2D(m, position.x_, position.y_);
	mw::rotate2D(m, angle);
	mw::scale2D(m, scale.x_, scale.y_);

	Vec2 p1 = m * Vec2(-0.5f, -0.5f);
	Vec2 p2 = m * Vec2(0.5f, -0.5f);
	Vec2 p3 = m * Vec2(0.5f, 0.5f);
	Vec2 p4 = m * Vec2(-0.5f, 0.5f);

	vd_->addSquareTRIANGLES(p1.x_, p1.y_,
		p2.x_, p2.y_,
		p3.x_, p3.y_,
		p4.x_, p4.y_,
		sprite,
		color);
}

void LightningBolt::draw(float deltaTime, const BoardShader& boardShader) {
	if (alpha_ <= 0)
		return;

	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

	halfCircle_.bindTexture();
	vd_->begin();

	alpha_ -= fadeOutRate_;

	for (Line& segment : segments_) {
		Color color = Tint_ * (alpha_ * alphaMultiplier_);
		addLine(segment.start_, segment.end_, color);
	}

	//update();

	vd_->end();

	lineSegment_.bindTexture();
	vd_->drawTRIANGLES(boardShader);
}

std::list<LightningBolt::Line> LightningBolt::createBolt(Vec2 source, Vec2 dest) {
	std::list<Line> results;
	Vec2 tangent = dest - source;
	Vec2 normal = Vec2(tangent.y_, -tangent.x_).normalize();
	float length = tangent.magnitude();

	std::vector<float> positions;
	positions.push_back(0);

	for (int i = 0; i < length / 4; i++)
		positions.push_back(rand(0, 1));

	std::sort(positions.begin(), positions.end(), [](float a, float b) {
		return a < b;
	});

	const float sway = 80;
	const float jaggedness = 1 / sway;

	Vec2 prevPoint = source;
	float prevDisplacement = 0;
	for (unsigned int i = 1; i < positions.size(); ++i) {
		float pos = positions[i];

		// used to prevent sharp angles by ensuring very close positions also have small perpendicular variation.
		float scale = (length * jaggedness) * (pos - positions[i - 1]);

		// defines an envelope. Points near the middle of the bolt can be further from the central line.
		float envelope = pos > 0.95f ? 20 * (1 - pos) : 1;

		float displacement = rand(-sway, sway);
		displacement -= (displacement - prevDisplacement) * (1 - scale);
		displacement *= envelope;

		Vec2 point = source + pos * tangent + displacement * normal;
		results.push_back(Line(prevPoint, point, thickness_));
		prevPoint = point;
		prevDisplacement = displacement;
	}

	results.push_back(Line(prevPoint, dest, thickness_));

	return results;
}

// Returns the point where the bolt is at a given fraction of the way through the bolt. Passing
// zero will return the start of the bolt, and passing 1 will return the end.
Vec2 LightningBolt::getPoint(float position) {
	Vec2 start = segments_.front().start_;
	Vec2 end = segments_.back().end_;
	float length = (end - start).magnitude();
	Vec2 dir = (end - start) / length;
	position *= length;

	Line line = findSegmentLargerThan(position, dir);
	float lineStartPos = (line.start_ - start) * dir;
	float lineEndPos = (line.start_ - start) * dir;
	float linePos = (position - lineStartPos) / (lineEndPos - lineStartPos);

	return lerp(line.start_, line.end_, linePos);
}


Vec2 LightningBolt::lerp(Vec2 p1, Vec2 p2, float value) {
	return p1 + (p2 - p1) * value;
}

float LightningBolt::rand(float min, float max) {
	std::uniform_real_distribution<float> dist_(min, max);
	return dist_(generator_);
}

LightningBolt::Line LightningBolt::findSegmentLargerThan(float position, Vec2 dir) const {
	Vec2 start = segments_.front().start_;
	std::list<Line> lines;
	for (const Line& line : segments_) {
		Vec2 p = line.end_;

		if ((p - start) * dir >= position) {
			return line;
		}
	}
	return segments_.back();
}
