#pragma once

#include "Mesh.h"
#include "Texture.h"

#define OPENDDL_STATIC_LIBARY

// Forward Declarations
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

namespace kuai {
	/** \class Model
	*   \brief A 3D object that is comprised of a collection of Meshes.
	*/
	class Model
	{
	public:
		/**
		* Load model from a 3D object file.
		*/
		Model(const std::string& filename);
		/**
		* Create model by specifying a singlular mesh and (optional) material.
		*/
		Model(Rc<Mesh> mesh, Rc<Material> = nullptr);

		/**
		* Returns reference to list of meshes this model is made of.
		*/
		std::vector<Rc<Mesh>>& getMeshes() { return meshes; }

		/**
		* Returns reference to list of materials associated with each mesh.
		*/
		std::vector<Rc<Material>>& getMaterials() { return materials; }

		void setMaterial(Rc<Material> material, u32 index) { materials[index] = material; }

	private:
		void processNode(aiNode* node, const aiScene* scene);
		Rc<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, uint64_t type);

	private:
		std::vector<Rc<Mesh>> meshes;
		std::vector<Rc<Material>> materials;

		std::string directory;
		std::unordered_map<std::string, Texture> loadedTexMap;
	};
}