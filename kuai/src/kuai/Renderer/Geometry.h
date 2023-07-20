#pragma once

#include "Model.h"

static const std::vector<float> quadPositions =
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f
};

static const std::vector<float> quadNormals =
{
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f
};

static const std::vector<float> quadTexCoords =
{
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f
};

static const std::vector<uint32_t> quadIndices =
{ 
	0, 1, 2, 
	2, 3, 0
};

namespace kuai {
	class Geometry
	{
	public:
		static Rc<Model> quad;

	private:
		static void init();

		friend class App;
	};
}