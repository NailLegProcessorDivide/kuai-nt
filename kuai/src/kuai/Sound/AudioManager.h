#pragma once

#include "AL/al.h"
#include "AL/alc.h"
#include "glm/glm.hpp"

#include "AudioSource.h"

// @cond
namespace kuai {
	class AudioManager
	{
	public:
		static void init();
		static void cleanup();

		static AudioSource* createAudioSource(bool stream = false);
		static void destroyAudioSource(u32 id);

		static float getGlobalGain();
		static void setGlobalGain(float gain);

		static void setPos(const glm::vec3& pos);
		static void setOrientation(const glm::vec3& at, const glm::vec3& up);

	private:
		static ALCdevice* device;
		static ALCcontext* context;

		static std::unordered_map<u32, AudioSource*> sourceMap;
	};
	
	// This is based on SFML - Simple and Fast Multimedia Library
	// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
	// The do-while loop is needed so that alCheck can be used as a single statement in if/else branches
	#define KU_DEBUG

	#ifdef KU_DEBUG
		#define alCheck(expr)                                      \
			do                                                     \
			{                                                      \
				expr;                                              \
				kuai::checkAlErrors(__FILE__, __LINE__, #expr);	   \
			} while (false)
	#else
		#define alCheck(expr) expr
	#endif

	void checkAlErrors(const std::filesystem::path& file, unsigned int line, std::string_view expression);
}
// @endcond
