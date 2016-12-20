#include "buffer.h"
#include "vertexdata.h"

Buffer::Buffer() : staticData_(true) {
}

Buffer::Buffer(bool staticData) : staticData_(staticData) {
}

void Buffer::uploadToGraphicCard() {
	std::vector<GLfloat> data;
	vbo_ = mw::VertexBufferObject();
	for (auto& vd : vertexDataList_) {
		vd->vbo_ = vbo_;
		vd->offsetInBytes_ = data.size() * sizeof(GLfloat);
		vd->maxVertexes_ = vd->data_.size() / vd->vertexSizeInFloat();
		data.insert(data.end(), vd->data_.begin(), vd->data_.end());
	}

	if (data.size() > 0) {
		vbo_.bindBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), staticData_ ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}

void Buffer::addVertexData(const VertexDataPtr& vd) {
	if (!vd->addedToBuffer_) {
		vd->addedToBuffer_ = true;
		vd->dynamic_ = !staticData_;
		vertexDataList_.push_back(vd);
	}
}

void Buffer::clearBuffer() {
	vbo_ = mw::VertexBufferObject();
	vertexDataList_.clear();
}
