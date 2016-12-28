#include "vertexdata.h"
#include "boardshader.h"

#include <mw/vertexbufferobject.h>

VertexData::VertexData() : maxVertexes_(0), offsetInBytes_(0), dynamic_(true), addedToBuffer_(false), index_(0) {
}

void VertexData::draw(GLenum mode) {
	int vertexes = index_ / vertexSizeInFloat();
	if (maxVertexes_ <= vertexes) {
		vertexes = maxVertexes_;
	}

	if (vbo_.getSize() > 0) {
		useProgram();
		vbo_.bindBuffer();
		setVertexAttribPointer();
		glDrawArrays(mode, offsetInBytes_ / vertexSizeInBytes(), vertexes);
		mw::checkGlError();
	}
}

void VertexData::begin() {
	index_ = 0;
}

void VertexData::end() {
	if (dynamic_) {
		int size = vertexSizeInBytes() * maxVertexes_;
		if (vbo_.getSize() > 0 && data_.size() * sizeof(GLfloat) <= size) {
			vbo_.bindBuffer();
			vbo_.bindBufferSubData(offsetInBytes_, data_.size() * sizeof(GLfloat), data_.data());
		}
	} else {
		index_ = data_.size();
		maxVertexes_ = data_.size() / vertexSizeInFloat();
	}
}
