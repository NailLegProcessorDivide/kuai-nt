#pragma once

namespace kuai {
	/** \class Texture
	*	\brief A 2D texture that can support transparency.
	*/
	class Texture {
	public:
		/**
		* Creates default blank texture.
		*/
		Texture();
		/**
		* Loads texture from image file.
		*/
		Texture(const std::string& filename);
		~Texture();
		/// @private
		u32 getId();
		/// @private
		void bind(u32 activeTex);
	private:
		u32 textureId;
	};
}