#include "dynamicbuffer.h"
#include "boardshader.h"

DynamicBuffer::DynamicBuffer() : DynamicBuffer(DEFAULT_VERTEXES_PER_OBJECT, DEFAUL_OBJECTS) {
}

DynamicBuffer::DynamicBuffer(int vertexesPerObject, int nbrObjects) :
	vertexesPerObject_(vertexesPerObject), nbrObjects_(nbrObjects), dynamic_(true) {

	for (int i = 0; i < nbrObjects_; ++i) {
		freeVertexDataList_.push_back(i);
	}
}

void DynamicBuffer::init() {
	std::vector<float> data(vertexesPerObject_ * nbrObjects_ * BoardShader::vertexSizeInFloat());
	vbo_.bindBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_DYNAMIC_DRAW);
}

void DynamicBuffer::free(const VertexDataPtr& vertexesPtr) {
	int index = vertexesPtr->offsetInBytes_ / BoardShader::vertexSizeInBytes() / vertexesPerObject_;
	freeVertexDataList_.push_back(index);
}

VertexDataPtr DynamicBuffer::pollFirstFree(int vertexes) {
	VertexDataPtr vertexDataPtr;
	for (int index : freeVertexDataList_) {
		vertexDataPtr = std::make_shared<VertexData>(vertexes, BoardShader::vertexSizeInBytes() * index * vertexesPerObject_, true, vbo_);
		freeVertexDataList_.remove(index);
		break;
	}
	if (!vertexDataPtr) {
		int a = 0;
	}

	return vertexDataPtr;
}

VertexDataPtr DynamicBuffer::pollFirstFree() {
	return DynamicBuffer::pollFirstFree(vertexesPerObject_);
}
