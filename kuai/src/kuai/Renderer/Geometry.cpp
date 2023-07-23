#include "kpch.h"

#include "Geometry.h"

namespace kuai {
	Rc<Mesh> Geometry::quad = nullptr;

	void Geometry::init()
	{
		quad = makeRc<Mesh>(quadVertexData, quadIndices);
	}
}