#pragma once

namespace kuai {
	/** \class Texture
	*	\brief A 2D texture that can support transparency.
	*/
	class Texture {
	public:
		enum class TextureFormat
		{
			RGB = 0,
			RGBA = 1
		};
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

		u32 getWidth() const { return width; }
		u32 getHeight() const { return height; }

	private:
		u32 textureId;

		u32 width = 1;
		u32 height = 1;
		TextureFormat format = TextureFormat::RGB;
	};
}