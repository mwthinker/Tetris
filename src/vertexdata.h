#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include "boardshader.h"
#include "color.h"

#include <mw/vertexbufferobject.h>

#include <vector>

class BoardShader;

class VertexData;
using VertexDataPtr = std::shared_ptr<VertexData>;

class VertexData {
public:
	friend class DynamicBuffer;
	friend class StaticBuffer;

	VertexData(int maxVertexes, int offsetInBytes, bool dynamic, const mw::VertexBufferObject& vbo);

	inline int getOffsetInBytes() const {
		return offsetInBytes_;
	}

	inline int getMaxVertexes() const {
		return maxVertexes_;
	}

	void drawTRIANGLES(const BoardShader& shader);

	void begin();

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	void addSquareTRIANGLES(
		float x, float y,
		float w, float h,
		const Color& color);

	void addSquareTRIANGLES(float x, float y, float w, float h, const mw::Sprite& sprite, const Color& color = Color(1, 1, 1));

	void addSquareTRIANGLES(float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float x4, float y4, 
		const mw::Sprite& sprite, const Color& color = Color(1, 1, 1));

	void addVertex(float x, float y, float xTex, float yTex, bool isTex, const Color& color);
	void updateVertex(float x, float y);

	void addTriangleTRIANGLES(
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		const Color& color);

	void end();

	void updateSquareTRIANGLES(float x, float y, float w, float h);

	void updateTriangleTRIANGLES(float x1, float y1,
		float x2, float y2,
		float x3, float y3);

	inline bool isDynamic() const {
		return dynamic_;
	}

private:
	bool dynamic_;
	int offsetInBytes_;
	int maxVertexes_;
	int index_;

	std::vector<GLfloat> data_;
	mw::VertexBufferObject vbo_;
};

#endif // VERTEXDATA_H
