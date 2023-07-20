#include "kpch.h"

#include "Geometry.h"

namespace kuai {
	Rc<Model> Geometry::quad = nullptr;

	void Geometry::init()
	{
		quad = makeRc<Model>(makeRc<Mesh>(quadPositions, quadNormals, quadTexCoords, quadIndices));
	}
}