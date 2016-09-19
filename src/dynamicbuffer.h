#ifndef GRAPHICBUFFER_H
#define GRAPHICBUFFER_H

#include "vertexdata.h"

#include <mw/vertexbufferobject.h>

#include <list>

class DynamicBuffer {
public:
	DynamicBuffer();

	void init();

	DynamicBuffer(int vertexes, int nbr);

	void free(const VertexDataPtr& vertexesPtr);

	VertexDataPtr pollFirstFree(int vertexes);

	VertexDataPtr pollFirstFree();

	inline int getNbrVertexes() const {
		return vertexes_;
	}

	inline int getMaxVertices() const {
		return vertexes_ / nbr_;
	}

	inline bool isDynamic() const {
		return dynamic_;
	}

private:
	bool dynamic_;
	int vertexes_;
	int nbr_;
	mw::VertexBufferObject vbo_;
	std::list<int> freeVertexDataList_;
	std::list<VertexDataPtr> vertexDataList_;
};

#endif // GRAPHICBUFFER_H
