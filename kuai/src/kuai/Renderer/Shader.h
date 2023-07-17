#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"

namespace kuai {

	class Shader
	{
	public:
		Shader(const std::string& vertSrc, const std::string& fragSrc);
		~Shader();

		void createUniform(const std::string& name);
		void setUniform(const std::string& name, int val) const;
		void setUniform(const std::string& name, float val) const;
		void setUniform(const std::string& name, const glm::vec2& val) const;
		void setUniform(const std::string& name, const glm::vec3& val) const;
		void setUniform(const std::string& name, const glm::vec4& val) const;
		void setUniform(const std::string& name, const glm::mat3& val) const;
		void setUniform(const std::string& name, const glm::mat4& val) const;

		void createUniformBlock(const std::string& name, const std::vector<const char*>& members, u32 binding);
		void setUniform(const std::string& name, const std::string& member, const void* data, u32 size) const;

		Rc<VertexArray> getVertexArray();

		u32 getCommandCount() const;
		void setIndirectBufData(const std::vector<IndirectCommand>& commands);

		void bind() const;
		void unbind() const;

	public:
		static void init();
		static void cleanup();

		static Shader* base;

	protected:
		int createShader(const char* src, int type);
		void link();

		int programId;
		int vertShaderId;
		int fragShaderId;

		std::unordered_map<std::string, u32> uniforms;

		Rc<VertexArray> vao;	    // Vertex array object
		Box<IndirectBuffer> ibo;	// Buffer containing mesh instance information

	private:
		// UBOs and member offsets are static as they are shared between all shaders
		static std::unordered_map<std::string, u32> ubos;
		static std::unordered_map<std::string, u32> uboOffsets;

		friend class StaticShader;
	};
}


