#ifndef STATICBUFFER_H
#define STATICBUFFER_H

#include "vertexdata.h"

#include <mw/vertexbufferobject.h>

#include <vector>

class StaticBuffer {
public:
	StaticBuffer();

	void init();

	VertexDataPtr pollVertexData();

private:
	bool wasInit_;
	mw::VertexBufferObject vbo_;
	std::vector<VertexDataPtr> vertexDataList_;
};

#endif // STATICBUFFER_H
