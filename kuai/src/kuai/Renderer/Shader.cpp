#include "kpch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace kuai {
	Shader* Shader::base = nullptr;

	std::unordered_map<std::string, u32> Shader::ubos = std::unordered_map<std::string, u32>();
	std::unordered_map<std::string, u32> Shader::uboOffsets = std::unordered_map<std::string, u32>();

	Shader::Shader(const std::string& vertSrc, const std::string& fragSrc)
	{
		programId = glCreateProgram();
		vertShaderId = createShader(vertSrc.c_str(), GL_VERTEX_SHADER);
		fragShaderId = createShader(fragSrc.c_str(), GL_FRAGMENT_SHADER);
		link();

		vao = makeRc<VertexArray>();
		ibo = makeBox<IndirectBuffer>(std::vector<IndirectCommand>());
	}

	Shader::~Shader()
	{
		unbind();
		
		for (auto& pair : ubos)
			glDeleteBuffers(1, &pair.second);

		if (programId)
			glDeleteProgram(programId);
	}

	void Shader::createUniform(const std::string& name)
	{
		int uniformLoc = glGetUniformLocation(programId, name.c_str());
		if (uniformLoc == -1)
			KU_CORE_ERROR("[Shader {0}] Could not find the uniform {1}", programId, name);
		uniforms[name] = uniformLoc;
	}

	void Shader::setUniform(const std::string& name, int val) const
	{
		glUniform1i(uniforms.at(name), val);
	}

	void Shader::setUniform(const std::string& name, float val) const
	{
		glUniform1f(uniforms.at(name), val);
	}

	void Shader::setUniform(const std::string& name, const glm::vec2& val) const
	{
		glUniform2f(uniforms.at(name), val.x, val.y);
	}

	void Shader::setUniform(const std::string& name, const glm::vec3& val) const
	{
		glUniform3f(uniforms.at(name), val.x, val.y, val.z);
	}

	void Shader::setUniform(const std::string& name, const glm::vec4& val) const
	{
		glUniform4f(uniforms.at(name), val.x, val.y, val.z, val.w);
	}

	void Shader::setUniform(const std::string& name, const glm::mat3& val) const
	{
		glUniformMatrix3fv(uniforms.at(name), 1, GL_FALSE, &val[0][0]);
	}

	void Shader::setUniform(const std::string& name, const glm::mat4& val) const
	{
		glUniformMatrix4fv(uniforms.at(name), 1, GL_FALSE, &val[0][0]);
	}

	void Shader::createUniformBlock(const std::string& name, const std::vector<const char*>& members, u32 binding)
	{
		// Get block index and block size
		u32 blockIndex = glGetUniformBlockIndex(programId, name.c_str());
		int blockSize;
		glGetActiveUniformBlockiv(programId, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

		// Get indices of member variables, and then their offsets
		GLuint* indices = new GLuint[members.size()];
		glGetUniformIndices(programId, members.size(), &members[0], indices);
		GLint* offsets = new GLint[members.size()];
		glGetActiveUniformsiv(programId, members.size(), indices, GL_UNIFORM_OFFSET, offsets);

		for (size_t i = 0; i < members.size(); i++)
			uboOffsets[members[i]] = offsets[i];

		delete[] indices;
		delete[] offsets;

		// Create uniform buffer object
		u32 ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);

		ubos[name] = ubo;
	}

	void Shader::setUniform(const std::string& name, const std::string& member, const void* data, u32 size) const
	{
		glNamedBufferSubData(ubos.at(name), uboOffsets.at(member), size, data);
	}

	Rc<VertexArray> Shader::getVertexArray()
	{
		return vao;
	}

	u32 Shader::getCommandCount() const
	{
		return ibo->getCount();
	}

	void Shader::setIndirectBufData(const std::vector<IndirectCommand>& commands)
	{
		ibo = makeBox<IndirectBuffer>(commands);
		ibo->bind();
	}

	void Shader::bind() const
	{
		glUseProgram(programId);
		vao->bind();
		ibo->bind();
	}

	void Shader::unbind() const
	{
		glUseProgram(0);
	}

	void Shader::init()
	{
		base = new Shader(
		R"(
		#version 450
		#define MAX_LIGHTS 10

		layout (location = 0)	in vec3 aPos;
		layout (location = 1)	in vec3 aNormal;
		layout (location = 2)	in vec2 aTexCoord;
		layout (location = 3)	in mat4 aModelMatrix;

		layout (binding = 0) uniform CamData
		{
			mat4 projMatrix;
			mat4 viewMatrix;
			vec3 viewPos;
		};

		out vec4 worldPos;
		out vec3 worldNorm;
		out vec2 texCoords;
		out vec3 viewingPos;

		void main()
		{
			worldPos = aModelMatrix * vec4(aPos, 1.0);
			mat3 model3x3InvTransp = mat3(transpose(inverse(aModelMatrix)));
			worldNorm = model3x3InvTransp * aNormal;
			texCoords = aTexCoord;
			viewingPos = viewPos;

			gl_Position = projMatrix * viewMatrix * worldPos;
		}
		)",
		R"(
		#version 450
		#define MAX_LIGHTS 10

		in vec4 worldPos;
		in vec3 worldNorm;
		in vec2 texCoords;
		in vec3 viewingPos;

		struct Light
		{
			int type;

			vec3 pos;
			vec3 dir;
			vec3 col;
    
			float intensity;
			float linear;
			float quadratic;
			float cutoff;
		};

		uniform sampler2D diffuse;

		layout (binding = 1) uniform Lights
		{
			Light lights[MAX_LIGHTS];
			int numLights;
		};

		out vec4 fragCol;

		void main()
		{
			fragCol = texture(diffuse, texCoords);
		}
		)");

		Rc<VertexBuffer> vbo1 = makeRc<VertexBuffer>(0);
		Rc<VertexBuffer> vbo2 = makeRc<VertexBuffer>(0);

		vbo1->setLayout(
			{
				{ ShaderDataType::VEC3, "pos" },
				{ ShaderDataType::VEC3, "normal" },
				{ ShaderDataType::VEC2, "texCoord" }
			});
		vbo2->setLayout(
			{
				{ ShaderDataType::MAT4,  "modelMatrix" }
			});
		base->vao->addVertexBuffer(vbo1);
		base->vao->addVertexBuffer(vbo2);

		base->bind();

		base->createUniformBlock("CamData", { "projMatrix", "viewMatrix", "viewPos" }, 0);

		std::vector<std::string> lightNames;
		std::vector<const char*> lightNamesCStr;
		for (u8 i = 0; i < 10; i++)	// TODO: HORRIBLE DISGUSTING CODE
		{
			lightNames.push_back("lights[" + std::to_string(i) + "].type");
			lightNames.push_back("lights[" + std::to_string(i) + "].pos");
			lightNames.push_back("lights[" + std::to_string(i) + "].dir");
			lightNames.push_back("lights[" + std::to_string(i) + "].col");
			lightNames.push_back("lights[" + std::to_string(i) + "].intensity");
			lightNames.push_back("lights[" + std::to_string(i) + "].linear");
			lightNames.push_back("lights[" + std::to_string(i) + "].quadratic");
			lightNames.push_back("lights[" + std::to_string(i) + "].cutoff");
		}
		for (auto& name : lightNames)
			lightNamesCStr.push_back(name.c_str());
		lightNamesCStr.push_back("numLights");
		base->createUniformBlock("Lights", lightNamesCStr, 1);

		base->createUniform("diffuse");
		base->setUniform("diffuse", 0);
	}

	void Shader::cleanup()
	{
		delete base;
	}

	int Shader::createShader(const char* src, int type)
	{
		int shaderId = glCreateShader(type);
		if (!shaderId)
			KU_CORE_ERROR("[Shader {0}] Failed to create shader ({1})", programId, type);

		glShaderSource(shaderId, 1, &src, nullptr);
		glCompileShader(shaderId);

		int compileSuccess;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileSuccess);
		if (!compileSuccess)
		{
			char errStr[1024];
			glGetShaderInfoLog(shaderId, 1024, nullptr, errStr); // Set max length of character buffer to 1024
			KU_CORE_ERROR("[Shader {0}] Error compiling shader code: {1}", programId, errStr);
		}

		glAttachShader(programId, shaderId);

		return shaderId;
	}

	void Shader::link()
	{
		glLinkProgram(programId);
		int linkSuccess;
		glGetProgramiv(programId, GL_LINK_STATUS, &linkSuccess);
		if (!linkSuccess)
		{
			char errStr[1024];
			glGetProgramInfoLog(programId, 1024, nullptr, errStr);
			KU_CORE_ERROR("[Shader {0}] Error linking shader code: {1}", programId, errStr);
		}

		if (vertShaderId)
			glDetachShader(programId, vertShaderId);
		if (fragShaderId)
			glDetachShader(programId, fragShaderId);

		glValidateProgram(programId);
		int validateSuccess;
		glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateSuccess);
		if (!validateSuccess)
		{
			char errStr[1024];
			glGetProgramInfoLog(programId, 1024, nullptr, errStr);
			//KU_CORE_ERROR("[Shader {0}] Error validating shader code: {1}", programId, errStr);
		}
	}
}