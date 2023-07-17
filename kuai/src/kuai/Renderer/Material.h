#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Cubemap.h"

#include <glm/glm.hpp>
#include <memory>

namespace kuai {
	/** \class Material 
	*   \brief Base class for materials. Every material has a shader that sets rendering properties in each render loop.
	*/
	class Material
	{
	public:
		Material() = default;

		virtual void bind(u32 offset) = 0;

		void setShader(Shader* shader) { this->shader = shader; };
		Shader* getShader() { return shader; }

	protected:
		Shader* shader = nullptr;
	};

	class DefaultMaterial : public Material
	{
	public:
		DefaultMaterial(Rc<Texture> diffuse) : diffuse(diffuse)
		{
			shader = Shader::base;
		}

		DefaultMaterial() : diffuse(new Texture())
		{
			shader = Shader::base;
		}

		virtual void bind(u32 offset)
		{
			diffuse->bind(offset);
		}

		void setDiffuse(Rc<Texture> diffuse) { this->diffuse = diffuse; }

		glm::vec2 getTiling() const { return tilingFactor; }
		void setTiling(float x, float y) { tilingFactor = glm::vec2(x, y); }

	private:
		Rc<Texture> diffuse;

		glm::vec2 tilingFactor = { 1.0f, 1.0f };

		friend class Model;
	};
}