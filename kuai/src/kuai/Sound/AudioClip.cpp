#include "kpch.h"
#include "AudioClip.h"

#include "AL/al.h"
#include "sndfile.hh"

namespace kuai {
	AudioClip::AudioClip(const std::string& filename) : handle(new SndfileHandle(filename))
	{
	}

	AudioClip::~AudioClip()
	{
		delete handle;
	}

	int AudioClip::getSamplerate()
	{
		return handle->samplerate();
	}

	int AudioClip::getChannels()
	{
		return handle->channels();
	}

	int AudioClip::getFrames()
	{
		return handle->frames();
	}

	uint64_t AudioClip::loadData(short* dest, uint64_t amount)
	{
		uint64_t readAmount = handle->read(dest, amount);
		if (readAmount < amount) // Set seek position to start if EOF reached
			handle->seek(0, SF_SEEK_SET);
		return readAmount;
	}

	std::vector<short> AudioClip::loadAllData()
	{
		std::vector<short> data(handle->channels() * handle->frames());
		handle->read(data.data(), data.size());
		return data;
	}

	int AudioClip::getFormat()
	{
		if (handle->channels() == 1 && handle->format() & SF_FORMAT_PCM_S8)
			return AL_FORMAT_MONO8;
		else if (handle->channels() == 1 && handle->format() & SF_FORMAT_PCM_16)
			return AL_FORMAT_MONO16;
		else if (handle->channels() == 2 && handle->format() & SF_FORMAT_PCM_S8)
			return AL_FORMAT_STEREO8;
		else if (handle->channels() == 2 && handle->format() & SF_FORMAT_PCM_16)
			return AL_FORMAT_STEREO16;
		
		return AL_NONE;
		//KU_CORE_WARN("Sound file {0} is stereo and will not have 3D positional sound", filename);
	}
}
