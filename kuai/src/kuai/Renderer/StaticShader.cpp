#include "kpch.h"
#include "StaticShader.h"

#include "kuai/Components/Components.h"

namespace kuai {
	Shader* StaticShader::basic = nullptr;
	
	const char* DEFAULT_VERT = R"(
		#version 450
		#define MAX_LIGHTS 10

		layout (location = 0)	in vec3 aPos;
		layout (location = 1)	in vec3 aNormal;
		layout (location = 2)	in vec2 aTexCoord;
		layout (location = 3)	in mat4 aModelMatrix;
		// layout (location = 7) in mat3 aModel3x3InvTransp;

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
			mat3 model3x3InvTransp = mat3(transpose(inverse(aModelMatrix))); // TODO: remove in favour of vertex attribute
			worldNorm = model3x3InvTransp * aNormal;
			texCoords = aTexCoord;
			viewingPos = viewPos;

			gl_Position = projMatrix * viewMatrix * worldPos;
		}
	)";

	const char* DEFAULT_FRAG = R"(
		#version 450
		#define MAX_LIGHTS 10

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

			int castsShadows;
		};

		layout (binding = 1) uniform Lights
		{
			Light lights[MAX_LIGHTS];
			int numLights;
		};

		out vec4 fragCol;

		in vec4 worldPos;
		in vec3 worldNorm;
		in vec2 texCoords;
		in vec3 viewingPos;

		struct Material
		{
			sampler2D diffuse;
			sampler2D specular;
			samplerCube reflectionMap;
			int reflections;
			float shininess;
		};
		uniform	Material materials[10];

		void main()
		{
			vec3 norm = normalize(worldNorm);
			vec3 viewDir = normalize(viewingPos - worldPos.xyz);
			vec3 viewReflectDir = reflect(-viewDir, norm);
			vec3 lightDir = vec3(0.0, 0.0, 0.0);
			float factor = 1.0;

			vec3 finalCol = vec3(0.0, 0.0, 0.0);

			for (int i = 0; i < numLights; i++)
			{
				if (lights[i].type == 0) // Directional Light
				{
					lightDir = normalize(-lights[i].dir);
					factor = lights[i].intensity;
				} 
				else
				{
					lightDir = normalize(lights[i].pos - worldPos.xyz);

					// Attenuation
					float distance = length(lights[i].pos - worldPos.xyz);
					factor = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * (distance * distance));    
					factor *= lights[i].intensity;

					if (lights[i].type == 2) // Spotlight
					{
						float theta = dot(lightDir, normalize(-lights[i].dir)); 
						float epsilon = 0.1;
						float intensity = clamp((theta - lights[i].cutoff) / epsilon, 0.0, 1.0);

						factor *= intensity;
					}
				}

				// Ambient
				vec3 ambient = 0.1 * lights[i].col * vec3(texture(materials[0].diffuse, texCoords));

				// Diffuse
				float diff = max(dot(norm, lightDir), 0.0);
				vec3 diffuse = diff * vec3(texture(materials[0].diffuse, texCoords));

				// Specular
				vec3 reflectDir = reflect(-lightDir, norm);
				vec3 halfwayDir = normalize(lightDir + viewDir);
				float spec = pow(max(dot(norm, halfwayDir), 0.0), materials[0].shininess);
				vec3 specular = lights[i].col * spec * vec3(texture(materials[0].specular, texCoords));
				 
				finalCol += (ambient + diffuse + specular) * factor;
			}

			fragCol = vec4(1.0, 0, 0, 1.0);
		}
	)";

	DefaultShader::DefaultShader() : Shader(DEFAULT_VERT, DEFAULT_FRAG) 
	{
		Rc<VertexBuffer> vbo1 = makeRc<VertexBuffer>(0);
		Rc<VertexBuffer> vbo2 = makeRc<VertexBuffer>(0);

		vbo1->setLayout(
		{ 
			{ ShaderDataType::VEC3, "pos" },
			{ ShaderDataType::VEC3, "normal" },
			{ ShaderDataType::VEC2, "texCoord" },
		});
		vbo2->setLayout(
		{
			{ ShaderDataType::MAT4,  "modelMatrix" }, // TODO: change to 3 Vec4s and use fact that bottom row of model matrix is always (0, 0, 0, 1)
			// { ShaderDataType::MAT3,  "model3x3InvTransp" }
		});
		vao->addVertexBuffer(vbo1);
		vao->addVertexBuffer(vbo2);

		bind();

		createUniformBlock("CamData", { "projMatrix", "viewMatrix", "viewPos" }, 0);
		
		std::vector<std::string> lightNames;
		std::vector<const char*> lightNamesCStr;
		for (u8 i = 0; i < MAX_LIGHTS; i++)	// TODO: HORRIBLE DISGUSTING CODE
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
		createUniformBlock("Lights", lightNamesCStr, 1);

		for (u8 i = 0; i < 10; i++)	
		{
			createUniform("materials[" + std::to_string(i) + "].diffuse");
			createUniform("materials[" + std::to_string(i) + "].specular");
			createUniform("materials[" + std::to_string(i) + "].reflectionMap");

			setUniform("materials[" + std::to_string(i) + "].diffuse", i * 3);
			setUniform("materials[" + std::to_string(i) + "].specular", i * 3 + 1);
			setUniform("materials[" + std::to_string(i) + "].reflectionMap", i * 3 + 2);

			createUniform("materials[" + std::to_string(i) + "].shininess");
			createUniform("materials[" + std::to_string(i) + "].reflections");
			setUniform("materials[" + std::to_string(i) + "].shininess", 20.0f);
			setUniform("materials[" + std::to_string(i) + "].reflections", 0);
		}
	}

	void StaticShader::init()
	{
		basic = new DefaultShader();
	}

	void StaticShader::cleanup()
	{
		//delete basic;
	}
}

