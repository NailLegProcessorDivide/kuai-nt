#include "kpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace kuai {

	Model::Model(const std::string& filename)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			KU_CORE_ERROR("Error loading model: {0}", importer.GetErrorString());
			return;
		}
		directory = filename.substr(0, filename.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	Model::Model(const Rc<Mesh>& mesh, const Rc<Material>& material)
	{
		meshes.push_back(mesh);
		if (!material)
		{
			// Default material
			Rc<Texture> tex = MakeRc<Texture>();
			materials.push_back(MakeRc<DefaultMaterial>(tex, tex, 10.0f));
		}
		else
			materials.push_back(material);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Rc<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertexData(mesh->mNumVertices);
		std::vector<u32> indices;
		std::vector<Texture*> textures;

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			vertexData[i].pos[0] = mesh->mVertices[i].x;
			vertexData[i].pos[1] = mesh->mVertices[i].y;
			vertexData[i].pos[2] = mesh->mVertices[i].z;

			vertexData[i].normal[0] = mesh->mNormals[i].x;
			vertexData[i].normal[1] = mesh->mNormals[i].x;
			vertexData[i].normal[2] = mesh->mNormals[i].x;

			if (mesh->mTextureCoords[0])
			{
				vertexData[i].texCoords[0] = mesh->mTextureCoords[0][i].x;
				vertexData[i].texCoords[1] = mesh->mTextureCoords[0][i].y;
			}

			vertexData[i].texId = 0;
		}

		// Indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Material
		if (mesh->mMaterialIndex)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			// TODO: normal maps and height maps
			if (textures.size() > 1)
			{
				materials.push_back(MakeRc<DefaultMaterial>(MakeRc<Texture>(*textures[0]), MakeRc<Texture>(*textures[1]), 20.0f));
				return MakeRc<Mesh>(vertexData, indices);
			}
		}

		materials.push_back(MakeRc<DefaultMaterial>(MakeRc<Texture>(), MakeRc<Texture>(), 20.0f));
		return MakeRc<Mesh>(vertexData, indices);
	}

	std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, uint64_t type)
	{
		std::vector<Texture*> textures;

		for (size_t i = 0; i < mat->GetTextureCount((aiTextureType)type); i++)
		{
			aiString str;
			mat->GetTexture((aiTextureType)type, i, &str);
			std::string filename = directory + "/" + str.C_Str();

			if (loadedTexMap.find(filename) != loadedTexMap.end())
			{
				textures.push_back(loadedTexMap[filename]);
			}
			else
			{
				Texture* tex = new Texture(filename);
				textures.push_back(tex);
				loadedTexMap.insert(std::pair<std::string, Texture*>(filename, tex));
			}
		}

		return textures;
	}

}
