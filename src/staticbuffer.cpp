#include "staticbuffer.h"
#include "boardshader.h"

StaticBuffer::StaticBuffer() : wasInit_(false) {
}

void StaticBuffer::init() {
	if (!wasInit_) {
		std::vector<GLfloat> data;
		for (VertexDataPtr vd : vertexDataList_) {
			vd->offsetInBytes_ = data.size() * sizeof(GLfloat);
			data.insert(data.end(), vd->data_.begin(), vd->data_.end());
			vd->data_.clear();
		}

		if (data.size() > 0) {
			vbo_.bindBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
		}

		vertexDataList_.clear();
	}
}

VertexDataPtr StaticBuffer::pollVertexData() {
	VertexDataPtr vertexDataPtr;
	if (!wasInit_) {
		vertexDataPtr = std::make_shared<VertexData>(0, 0, false, vbo_);
		vertexDataList_.push_back(vertexDataPtr);
	}
	return vertexDataPtr;
}
