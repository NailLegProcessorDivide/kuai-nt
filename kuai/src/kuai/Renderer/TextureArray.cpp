#include "kpch.h"

#include "TextureArray.h"

#include "glad/glad.h"

namespace kuai {

	TextureArray::TextureArray(u32 width, u32 height, u32 layers)
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

		// Allocate the storage; only use 1 mip level
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layers);
		// First zero is mipmap level; next two zeros are x and y offsets; last zero is layer index offset
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layers, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// Set texture wrapping options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set filtering options for down/upscaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	TextureArray::~TextureArray()
	{
		glDeleteTextures(GL_TEXTURE_2D_ARRAY, &textureId);
	}

	u32 TextureArray::getId()
	{
		return textureId;
	}

	void TextureArray::bind(u32 activeTex)
	{
		glActiveTexture(GL_TEXTURE0 + activeTex);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
	}
}