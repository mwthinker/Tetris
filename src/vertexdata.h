#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <mw/vertexbufferobject.h>

#include <vector>
#include <array>

#include <cassert>

class VertexData;
using VertexDataPtr = std::shared_ptr<VertexData>;

class VertexData {
public:
	friend class Buffer;

	VertexData();

	inline virtual ~VertexData() {
	}

	inline unsigned int getOffsetInBytes() const {
		return offsetInBytes_;
	}

	inline unsigned int getMaxVertexes() const {
		return maxVertexes_;
	}

	virtual unsigned int vertexSizeInFloat() const = 0;

	unsigned int vertexSizeInBytes() const {
		return vertexSizeInFloat() * sizeof(GLfloat);
	}

	virtual void setVertexAttribPointer() const = 0;

	virtual void useProgram() const = 0;

	void draw(GLenum mode);

	void begin();
	void end();

	inline bool isDynamic() const {
		return dynamic_;
	}

	template <unsigned int N>
	void addVertex(const std::array<float, N>& vertex) {
		// Vertex size must equal the provided vertex size.
		assert(vertex.size() == vertexSizeInFloat());
		if (dynamic_) {
			if (vbo_.getSize() == 0) {
				if (index_ + vertex.size() <= data_.size()) {
					for (float value : vertex) {
						data_[index_++] = value;
					}
				} else {
					for (float value : vertex) {
						data_.push_back(value);
					}
					index_ += vertexSizeInFloat();
				}
			} else if (index_ + vertexSizeInFloat() <= maxVertexes_ * vertexSizeInFloat()) {
				if (index_ < data_.size()) {
					for (float value : vertex) {
						data_[index_++] = value;
					}
				} else {
					for (float value : vertex) {
						data_.push_back(value);
					}
					index_ += vertexSizeInFloat();
				}
			}
		} else {
			for (float value : vertex) {
				data_.push_back(value);
			}
			index_ += vertexSizeInFloat();
		}
	}

	bool isAddedToBuffer() const {
		return addedToBuffer_;
	}

protected:
	inline void nextVertex() {
		index_ += vertexSizeInFloat();
	}

	inline void updateVertex(unsigned int index, float value) {
		data_[index_ + index] = value;
	}

private:
	bool addedToBuffer_;
	bool dynamic_;
	int offsetInBytes_;
	unsigned int maxVertexes_;
	unsigned int index_;

	std::vector<GLfloat> data_;
	mw::VertexBufferObject vbo_;
};

#endif // VERTEXDATA_H
