#pragma once

namespace kuai {
	/** \class Cubemap
	*	\brief A collection of six textures that form a cube. Used in environment mapping and to create Skybox objects.
	*/
	class Cubemap
	{
	public:
		/**
		*	@param faces A list of filenames that correspond to each face of the cube. Order of faces : px, nx, py, ny, pz, nz.
		*/
		Cubemap(const std::vector<std::string>& faces);
		~Cubemap();

		uint32_t getId();
		/// @private
		void bind(uint32_t activeTex);
	private:
		uint32_t textureId;
	};

}

