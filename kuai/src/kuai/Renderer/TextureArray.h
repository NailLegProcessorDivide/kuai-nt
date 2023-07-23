#pragma once

#include "Texture.h"

namespace kuai {
	class TextureArray
	{
	public:
		TextureArray(u32 width, u32 height, u32 layers);
		~TextureArray();

		void insert(Rc<Texture> texture);
		void remove(Rc<Texture> texture);

		u32 getId();

		void bind(u32 activeTex);

	private:
		u32 textureId;

		u32 width, height;
		u32 layers;

		std::unordered_map<u32, u32> texMap;
	};
}