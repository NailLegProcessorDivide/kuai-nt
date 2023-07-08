#pragma once

#include "kuai/Components/System.h"

namespace kuai {

	class RenderSystem : public System
	{
	public:
		void update(float dt)
		{
			// Update model matrices every frame
			// TODO: inefficient, only update when transform moves
			for (auto& pair : shaderToEntities)
			{
				std::vector<glm::mat4> modelMatrices;
				for (auto& innerPair : pair.second)
				{
					modelMatrices.push_back(ECS->getComponent<Transform>(innerPair.first).getModelMatrix());
				}
				shaderToModelMatrices[pair.first] = modelMatrices;
			}

			render();
		}

		void insertEntity(EntityID id) override
		{
			System::insertEntity(id);

			Rc<Model> model = ECS->getComponent<MeshRenderer>(id).getModel();

			// For every mesh in the model
			for (size_t i = 0; i < model->getMeshes().size(); i++)
			{
				Rc<Mesh> mesh = model->getMeshes()[i];
				u32 meshId = mesh->getId();
				Shader* shader = model->getMaterials()[i]->getShader();

				IndirectCommand& cmd = shaderToMeshCommand[shader][meshId];
				cmd.instanceCount = ++shaderMeshToNumInstances[shader][meshId]; // Increment instance count by one

				if (cmd.instanceCount == 1) // First instance, so add its vertex data and indices to back of the list
				{
					std::vector<Vertex>& vertexData = shaderToVertexData[shader];
					std::vector<u32>& indices = shaderToIndices[shader];
					// Setup this mesh's render command
					cmd.count = mesh->indices.size();				// Number of indices mesh uses
					cmd.firstIndex = indices.size();				// Offset of first index
					cmd.baseVertex = vertexData.size();				// Offset of first vertex
					cmd.baseInstance = shaderToInstances[shader];	// Offset of first instance

					shaderToVertexDataSizes[shader][meshId] = mesh->vertexData.size();
					shaderToIndicesSizes[shader][meshId] = mesh->indices.size();
					// Insert the data at the end
					vertexData.insert(vertexData.end(), mesh->vertexData.begin(), mesh->vertexData.end());
					indices.insert(indices.end(), mesh->indices.begin(), mesh->indices.end());

					dataChanged = true;
				}

				shaderToEntities[shader][id] = 1;

				shaderToMeshCommand[shader][meshId] = cmd;

				shaderToInstances[shader]++;
			}

			// Resize model matrices and set commands
			for (auto& pair : shaderToInstances)
			{
				pair.first->getVertexArray()->getVertexBuffers()[1]->reset(nullptr, pair.second * sizeof(glm::mat4), DrawHint::DYNAMIC);
			}
			setCommands();
		}

		void removeEntity(EntityID id) override
		{
			Rc<Model> model = ECS->getComponent<MeshRenderer>(id).getModel();

			for (size_t i = 0; i < model->getMeshes().size(); i++)
			{
				Rc<Mesh> mesh = model->getMeshes()[i];
				u32 meshId = mesh->getId();
				Shader* shader = model->getMaterials()[i]->getShader();

				IndirectCommand& cmd = shaderToMeshCommand[shader][meshId];
				cmd.instanceCount = --shaderMeshToNumInstances[shader][meshId]; // Decrement instance count by one

				if (cmd.instanceCount == 0) // No more instances of this mesh left -> remove its vertex data and indices from lists
				{
					std::vector<Vertex>& vertexData = shaderToVertexData[shader];
					std::vector<u32>& indices = shaderToIndices[shader];

					size_t vertexDataSize = shaderToVertexDataSizes[shader][meshId];
					size_t indicesSize = shaderToIndicesSizes[shader][meshId];

					vertexData.erase(vertexData.begin() + cmd.baseVertex, vertexData.begin() + cmd.baseVertex + vertexDataSize);
					indices.erase(indices.begin() + cmd.firstIndex, indices.begin() + cmd.firstIndex + indicesSize);

					// Take away vertex data size and indices size from offset for all meshes that are further along in the list
					for (auto& pair : shaderToMeshCommand)
					{
						for (auto& innerPair : pair.second)
						{
							if (innerPair.second.baseInstance > cmd.baseInstance)
							{
								innerPair.second.baseVertex -= vertexDataSize;
								innerPair.second.firstIndex -= indicesSize;
							}
						}
					}

					shaderToVertexDataSizes[shader].erase(meshId);
					shaderToIndicesSizes[shader].erase(meshId);
					shaderMeshToNumInstances[shader].erase(meshId);

					dataChanged = true;
				}
				// Decrement base instances of all meshes that are further along the list as we deleted an instance 
				for (auto& pair : shaderToMeshCommand)
				{
					for (auto& innerPair : pair.second)
					{
						if (innerPair.second.baseInstance > cmd.baseInstance)
						{
							innerPair.second.baseInstance--;
						}
					}
				}

				if (cmd.instanceCount == 0)
				{
					shaderToMeshCommand[shader].erase(meshId); // Remove the render command
				}
	
				shaderToEntities[shader].erase(id);

				shaderToInstances[shader]--;
			}

			// Resize model matrices and set commands
			for (auto& pair : shaderToInstances)
			{
				pair.first->getVertexArray()->getVertexBuffers()[1]->reset(nullptr, pair.second * sizeof(glm::mat4), DrawHint::DYNAMIC);
			}
			setCommands();

			System::removeEntity(id);
		}

		void setCommands()
		{
			int i = 0;
			for (auto& pair : shaderToMeshCommand)
			{
				pair.first->bind();
				std::vector<IndirectCommand> commands;
				for (auto& innerPair : pair.second)
				{
					commands.push_back(innerPair.second);
				}

				pair.first->setIndirectBufData(commands);
				i++;
			}
		}

		void render()
		{
			// ---- SHADOWS ----
			//if (scene->getMainLight().castsShadows())
			//{
			//	auto& basicShaderVertexData = shaderToVertexData[StaticShader::basic];
			//	auto& basicShaderIndices = shaderToIndices[StaticShader::basic];
			//	auto& basicShaderModelMatrices = shaderToModelMatrices[StaticShader::basic];

			//	StaticShader::depth->bind();

			//	StaticShader::depth->getVertexArray()->getVertexBuffers()[0]->reset(basicShaderVertexData.data(), basicShaderVertexData.size() * sizeof(Vertex));
			//	StaticShader::depth->getVertexArray()->setIndexBuffer(MakeRc<IndexBuffer>(basicShaderIndices.data(), basicShaderIndices.size()));
			//	StaticShader::depth->getVertexArray()->getVertexBuffers()[1]->reset(basicShaderModelMatrices.data(), basicShaderModelMatrices.size() * sizeof(glm::mat4));

			//	std::vector<IndirectCommand> commands;
			//	for (auto& pair : shaderToMeshCommand[StaticShader::basic])
			//		commands.push_back(pair.second);
			//	StaticShader::depth->setIndirectBufData(commands);

			//	Renderer::updateShadowMap(scene->getMainLight());
			//}

			// -----------------

			Renderer::setViewport(0, 0, App::get().getWindows()[0]->getWidth(), App::get().getWindows()[0]->getHeight());
			Renderer::clear();

			for (auto& pair : shaderToInstances)
			{
				Shader* shader = pair.first;
				shader->bind();

				for (auto& pair : shaderToEntities[shader])
				{
					Rc<Model> model = ECS->getComponent<MeshRenderer>(pair.first).getModel();

					for (int i = 0; i < model->getMeshes().size(); i++)
					{
						// TODO: change this to a big texture array or something that doesn't send possibly duplicate materials
						Rc<Mesh> mesh = model->getMeshes()[i];
						Rc<Material> material = model->getMaterials()[i];

						material->bind(0);
					}
				}

			RENDER_LABEL:

				if (dataChanged)
				{
					auto& vertexData = shaderToVertexData[shader];
					shader->getVertexArray()->getVertexBuffers()[0]->reset(vertexData.data(), vertexData.size() * sizeof(Vertex), DrawHint::DYNAMIC);

					auto& indices = shaderToIndices[shader];

					shader->getVertexArray()->setIndexBuffer(makeRc<IndexBuffer>(indices.data(), indices.size()));
				}
				else
				{
					auto& modelMatrices = shaderToModelMatrices[shader];
					shader->getVertexArray()->getVertexBuffers()[1]->setData(modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));
				}

				Renderer::render(shader);
			}
			dataChanged = false;
		}

	private:
		// Maps shader to entities within its control
		std::unordered_map<Shader*, std::unordered_map<u32, u32>> shaderToEntities;
		// Total number of instances shader owns
		std::unordered_map<Shader*, size_t> shaderToInstances;

		// Maps shader and mesh to the number of instances they have.
		std::unordered_map<Shader*, std::unordered_map<u32, size_t>> shaderMeshToNumInstances;

		// Map each shader to a list of vertices, indices and model matrices.
		std::unordered_map<Shader*, std::vector<Vertex>> shaderToVertexData;
		std::unordered_map<Shader*, std::vector<u32>> shaderToIndices;
		std::unordered_map<Shader*, std::vector<glm::mat4>> shaderToModelMatrices;

		// For each shader and for each mesh, store the sizes of their vertex and index lists.
		std::unordered_map<Shader*, std::unordered_map<u32, size_t>> shaderToVertexDataSizes;
		std::unordered_map<Shader*, std::unordered_map<u32, size_t>> shaderToIndicesSizes;

		// For each shader and for each mesh, store its corresponding render command.
		std::unordered_map<Shader*, std::unordered_map<u32, IndirectCommand>> shaderToMeshCommand;

		bool dataChanged = false;
	};
}