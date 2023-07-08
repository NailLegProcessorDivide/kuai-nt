#pragma once

static float[] quadPositions = 
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
};

static float[] quadTexCoords = 
{
	0, 1,
	1, 1,
	1, 0,
	0, 0
};

static int[] quadIndices = { 0, 1, 2, 2, 3, 0, };

namespace kuai {
	class Geometry
	{
	public:

	};
}