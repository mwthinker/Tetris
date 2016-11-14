#ifndef GRAPHICBUFFER_H
#define GRAPHICBUFFER_H

#include "vertexdata.h"

#include <mw/vertexbufferobject.h>

#include <list>

class DynamicBuffer {
public:
	DynamicBuffer();

	DynamicBuffer(int vertexesPerObject, int nbrObjects);

	void init();

	void free(const VertexDataPtr& vertexesPtr);

	VertexDataPtr pollFirstFree(int vertexes);

	VertexDataPtr pollFirstFree();

	inline int getNbrObjects() const {
		return nbrObjects_;
	}

	inline int getVertexesPerObject() const {
		return vertexesPerObject_;
	}

	inline bool isDynamic() const {
		return dynamic_;
	}

private:
	static const int DEFAULT_VERTEXES_PER_OBJECT = 10000;
	static const int DEFAUL_OBJECTS = 1000;

	bool dynamic_;
	int nbrObjects_;
	int vertexesPerObject_;
	mw::VertexBufferObject vbo_;
	std::list<int> freeVertexDataList_;
	std::list<VertexDataPtr> vertexDataList_;
};

#endif // GRAPHICBUFFER_H
