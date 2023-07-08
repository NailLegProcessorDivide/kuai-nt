#pragma once

#include "kuai/Components/Components.h"

#include "glm/glm.hpp"

namespace kuai {
	class Renderer
	{
	public:
		static void init();
		static void cleanup();
		
		static void setCamera(Camera& camera);

		static void render(Shader* shader);

		static void setViewport(u32 x, u32 y, u32 width, u32 height);
		static void setClearCol(const glm::vec4& col);
		static void clear();

	private:
		struct RenderData
		{
			glm::mat4 projMatrix;
			glm::mat4 viewMatrix;
		};

		static Box<RenderData> renderData;
	};
}

