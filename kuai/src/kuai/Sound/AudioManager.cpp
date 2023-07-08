#include "kpch.h"

#include "AudioManager.h"
#include "AudioClip.h"
#include "MusicSource.h"

namespace kuai {
	ALCdevice* AudioManager::device = nullptr;
	ALCcontext* AudioManager::context = nullptr;

	std::unordered_map<u32, AudioSource*> AudioManager::sourceMap = std::unordered_map<u32, AudioSource*>();

	void AudioManager::init()
	{
		device = alcOpenDevice(nullptr);
		if (!device)
		{
			KU_CORE_ERROR("Failed to open playback device");
			exit(1);
		}

		// Creating a context automatically creates a listener object
		context = alcCreateContext(device, nullptr);
		if (!alcMakeContextCurrent(context))
		{
			KU_CORE_ERROR("Failed to make audio context current");
			exit(1);
		}

		KU_CORE_INFO("Started Audio Manager");
	}

	void AudioManager::cleanup()
	{
		for (auto& pair : sourceMap)
		{
			auto& source = pair.second;
			source->cleanup();
			delete source;
		}

		sourceMap.clear();

		if (context)
		{
			device = alcGetContextsDevice(context);
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(context);
		}

		alcCloseDevice(device);
	}

	AudioSource* AudioManager::createAudioSource(bool stream)
	{
		AudioSource* source = nullptr;
		if (stream)
		{
			source = new MusicSource();
		}
		else
		{
			source = new AudioSource();
		}

		sourceMap.emplace(source->getId(), source);

		return source;
	}

	void AudioManager::destroyAudioSource(u32 id)
	{
		if (context && !sourceMap.empty())
		{
			auto& source = sourceMap.at(id);

			source->cleanup();

			sourceMap.erase(id);
		}
	}

	float AudioManager::getGlobalGain()
	{
		ALfloat gain;
		alCheck(alGetListenerf(AL_GAIN, &gain));
		return gain;
	}

	void AudioManager::setGlobalGain(float gain)
	{
		alCheck(alListenerf(AL_GAIN, gain));
	}

	void AudioManager::setPos(const glm::vec3& pos)
	{
		alCheck(alListener3f(AL_POSITION, pos.x, pos.y, pos.z));
	}

	void AudioManager::setOrientation(const glm::vec3& at, const glm::vec3& up)
	{
		float vals[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
		alCheck(alListenerfv(AL_ORIENTATION, vals));
	}

	void checkAlErrors(const std::filesystem::path& file, unsigned int line, std::string_view expression)
	{
		// Get the last error
		ALenum errorCode = alGetError();

		if (errorCode != AL_NO_ERROR)
		{
			std::string error = "Unknown error";
			std::string description = "No description";

			// Decode the error code
			switch (errorCode)
			{
			case AL_INVALID_NAME:
			{
				error = "AL_INVALID_NAME";
				description = "A bad name (ID) has been specified.";
				break;
			}

			case AL_INVALID_ENUM:
			{
				error = "AL_INVALID_ENUM";
				description = "An unacceptable value has been specified for an enumerated argument.";
				break;
			}

			case AL_INVALID_VALUE:
			{
				error = "AL_INVALID_VALUE";
				description = "A numeric argument is out of range.";
				break;
			}

			case AL_INVALID_OPERATION:
			{
				error = "AL_INVALID_OPERATION";
				description = "The specified operation is not allowed in the current state.";
				break;
			}

			case AL_OUT_OF_MEMORY:
			{
				error = "AL_OUT_OF_MEMORY";
				description = "There is not enough memory left to execute the command.";
				break;
			}
			}

			// Log the error
			KU_CORE_ERROR("An internal OpenAL call failed in {0}({1}).\nExpression:\n  {2}\nError Description:\n  {3} - {4}",
				file.filename(), line, expression, error, description);
		}
	}
}
