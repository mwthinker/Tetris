#include "lightningbolt.h"

LightningBolt::LightningBolt(const LightningShader& lightShader, Vec2 source, Vec2 dest, mw::Sprite halfCircle, mw::Sprite  lineSegment, Color tInt, float alphaMultiplier, float fadeOutRate, float thickness) :
	LightningVertexData(lightShader),
	generator_(rd_()),
	Tint_(tInt),
	alpha_(1.f),
	alphaMultiplier_(alphaMultiplier),
	fadeOutRate_(fadeOutRate),
	thickness_(thickness),
	halfCircle_(halfCircle),
	lineSegment_(lineSegment) {

	segments_ = createBolt(source, dest);

	begin();
	for (auto& line : segments_) {
		addSegmentTRIANGLES(line.start_, line.end_, thickness_, halfCircle_, lineSegment_, halfCircle_);
	}
	end();
}

LightningBolt::LightningBolt(const LightningShader& lightShader, Vec2 source, Vec2 dest, mw::Sprite halfCircle, mw::Sprite lineSegment)
	: LightningBolt(lightShader, source, dest, halfCircle, lineSegment, Color(1,1,1), 0.8f, 0.005f, 8) {
}

void LightningBolt::addLine(Vec2 start, Vec2 end, const Color& color) {
	addSegmentTRIANGLES(start, end, thickness_, halfCircle_, lineSegment_, halfCircle_);
}

void LightningBolt::draw(float deltaTime) {
	if (alpha_ <= 0)
		return;

	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

	halfCircle_.bindTexture();
	
	alpha_ -= fadeOutRate_;

	Color color = Tint_ * (alpha_ * alphaMultiplier_);
	setColor(color);
	
	lineSegment_.bindTexture();
	LightningVertexData::draw(GL_TRIANGLES);
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