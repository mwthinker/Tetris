#include "dynamicbuffer.h"
#include "boardshader.h"

DynamicBuffer::DynamicBuffer() : vertexes_(1000 * 1000), nbr_(1000), dynamic_(true) {
	for (int i = 0; i < nbr_; ++i) {
		freeVertexDataList_.push_back(i);
	}
}

void DynamicBuffer::init() {
	std::vector<float> data(vertexes_ * BoardShader::vertexSizeInFloat());
	vbo_.bindBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_DYNAMIC_DRAW);
}

DynamicBuffer::DynamicBuffer(int vertexes, int nbr) {
	vertexes_ = vertexes / nbr;
	for (int i = 0; i < nbr; ++i) {
		freeVertexDataList_.push_back(nbr);
	}
}

void DynamicBuffer::free(VertexDataPtr vertexesPtr) {
	freeVertexDataList_.remove(vertexesPtr->offsetInBytes_ / BoardShader::vertexSizeInBytes());
}

VertexDataPtr DynamicBuffer::pollFirstFree(int vertexes) {
	VertexDataPtr vertexDataPtr;
	for (int index : freeVertexDataList_) {
		vertexDataPtr = std::make_shared<VertexData>(vertexes, BoardShader::vertexSizeInBytes() * index * vertexes_ / nbr_, true, vbo_);
		freeVertexDataList_.remove(index);
		break;
	}
	if (!vertexDataPtr) {
		int a = 0;
	}

	return vertexDataPtr;
}

VertexDataPtr DynamicBuffer::pollFirstFree() {	
	return DynamicBuffer::pollFirstFree(vertexes_ / nbr_);
}
