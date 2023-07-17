#include "kpch.h"
#include "Cubemap.h"

#include "stb_image.h"
#include "glad/glad.h"

namespace kuai {

	// Order of faces: px, nx, py, ny, pz, nz
	Cubemap::Cubemap(const std::vector<std::string>& faces)
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

		// Load file using stbi library
		int width, height, colourChannels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data;
		
		for (size_t i = 0; i < faces.size(); i++)
		{
			data = stbi_load(faces[i].c_str(), &width, &height, &colourChannels, 0);

			if (data)
			{
				GLenum format = 0;
				if (colourChannels == 1)
					format = GL_RED;
				else if (colourChannels == 3)
					format = GL_RGB;
				else if (colourChannels == 4)
					format = GL_RGBA;

				KU_CORE_ASSERT(format, "Texture file format not supported");

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);

				// Set texture wrapping options
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				// Set filtering options for down/upscaling
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				KU_CORE_INFO("Loaded cubemap texture: {0} ({1}x{2})", faces[i], width, height);
			}
			else
			{
				KU_CORE_ERROR("Failed to load cubemap texture: {0}", faces[i]);
				stbi_image_free(data);
			}
		}
	}

	Cubemap::~Cubemap()
	{
		glDeleteTextures(1, &textureId);
	}

	u32 Cubemap::getId()
	{
		return textureId;
	}
	
	void Cubemap::bind(u32 activeTex)
	{
		glActiveTexture(GL_TEXTURE0 + activeTex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	}
}