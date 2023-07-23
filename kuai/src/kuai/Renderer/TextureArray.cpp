#include "kpch.h"

#include "TextureArray.h"

#include "glad/glad.h"

#include "stb_image_resize.h"

namespace kuai {

	TextureArray::TextureArray(u32 width, u32 height, u32 layers) 
		: width(width), height(height), layers(layers)
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

		// Allocate the storage; only use 1 mip level
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layers);

		// Set texture wrapping options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set filtering options for down/upscaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	TextureArray::~TextureArray()
	{
		// glDeleteTextures(GL_TEXTURE_2D_ARRAY, &textureId);
	}

	void TextureArray::insert(Rc<Texture> texture)
	{
		if (texMap.find(texture->getId()) != texMap.end())
		{
			return;
		}

		texMap[texture->getId()]++;

		unsigned char* outData = new unsigned char[(size_t) width * height * 4];

		if (texture->getWidth() > width || texture->getHeight() > height)
		{
			unsigned char* inData = new unsigned char[(size_t)texture->getWidth() * texture->getHeight() * 4];

			glBindTexture(GL_TEXTURE_2D, texture->getId());
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, inData);

			stbir_resize_uint8(inData, texture->getWidth(), texture->getHeight(), 0, outData, width, height, 0, 4);

			delete[] inData;
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, texture->getId());
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData);
		}

		glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
		// First zero is mipmap level; next two zeros are x and y offsets; last zero is layer index offset
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture->getId(), width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, outData);

		delete[] outData;
	}

	void TextureArray::remove(Rc<Texture> texture)
	{
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