#pragma once

namespace kuai {
	class TextureArray
	{
	public:
		TextureArray(u32 width, u32 height, u32 layers);
		~TextureArray();

		u32 getId();

		void bind(u32 activeTex);

	private:
		u32 textureId;
	};
}