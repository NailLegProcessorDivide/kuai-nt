#include "kpch.h"

#include "AudioSource.h"
#include "AudioClip.h"
#include "AudioManager.h"

namespace kuai {
	AudioSource::AudioSource()
	{
		alCheck(alGenSources(1, &sourceId));
		alCheck(alGenBuffers(BUF_COUNT, buffers));
	}

	void AudioSource::cleanup()
	{
		alCheck(alSourceStop(sourceId));

		// Delete the buffers (This gets handled centrally by the AudioManager class)
		alCheck(alSourcei(sourceId, AL_BUFFER, 0));
		alCheck(alDeleteBuffers(BUF_COUNT, buffers));

		alCheck(alDeleteSources(1, &sourceId));
	}

	void AudioSource::play()
	{
		alCheck(alSourcePlay(sourceId));
	}

	void AudioSource::pause()
	{
		alCheck(alSourcePause(sourceId));
	}

	void AudioSource::stop()
	{
		alCheck(alSourceStop(sourceId));
	}

	void AudioSource::setAudioClip(Rc<AudioClip> audioClip)
	{
		this->audioClip = audioClip;

		std::vector<short> data = audioClip->loadAllData();

		alCheck(alSourcei(sourceId, AL_BUFFER, AL_NONE));
		alCheck(alBufferData(buffers[0], audioClip->getFormat(), data.data(), data.size() * sizeof(short), audioClip->getSamplerate()));
		alCheck(alSourcei(sourceId, AL_BUFFER, buffers[0]));

		data.clear();
	}

	float AudioSource::getGain() const
	{
		ALfloat gain;
		alCheck(alGetSourcef(sourceId, AL_GAIN, &gain));
		return gain;
	}

	void AudioSource::setGain(float gain)
	{
		alCheck(alSourcef(sourceId, AL_GAIN, gain));
	}

	float AudioSource::getPitch() const
	{
		ALfloat pitch;
		alCheck(alGetSourcef(sourceId, AL_PITCH, &pitch));
		return pitch;
	}

	void AudioSource::setPitch(float pitch)
	{
		alCheck(alSourcef(sourceId, AL_PITCH, pitch));
	}

	float AudioSource::getRolloff() const
	{
		ALfloat rolloff;
		alCheck(alGetSourcef(sourceId, AL_ROLLOFF_FACTOR, &rolloff));
		return rolloff;
	}

	void AudioSource::setRolloff(float rolloff)
	{
		alCheck(alSourcef(sourceId, AL_ROLLOFF_FACTOR, rolloff));
	}

	float AudioSource::getRefDist() const
	{
		ALfloat refDist;
		alCheck(alGetSourcef(sourceId, AL_REFERENCE_DISTANCE, &refDist));
		return refDist;
	}

	void AudioSource::setRefDist(float refDist)
	{
		alCheck(alSourcef(sourceId, AL_REFERENCE_DISTANCE, refDist));
	}

	bool AudioSource::isLoop() const
	{
		return loop;
	}

	void AudioSource::setLoop(bool loop)
	{
		this->loop = loop;
		alCheck(alSourcei(sourceId, AL_LOOPING, loop));
	}

	AudioSource::PlaybackState AudioSource::getStatus() const
	{
		ALint status;
		alCheck(alGetSourcei(sourceId, AL_SOURCE_STATE, &status));

		switch (status)
		{
			case AL_INITIAL:
			case AL_STOPPED:
				return PlaybackState::Stopped;
			case AL_PAUSED:
				return PlaybackState::Paused;
			case AL_PLAYING:
				return PlaybackState::Playing;
		}

		return PlaybackState::Stopped;
	}

	void AudioSource::setPos(const glm::vec3& pos)
	{
		alCheck(alSource3f(sourceId, AL_GAIN, pos.x, pos.y, pos.z));
	}

	void AudioSource::setDir(const glm::vec3& dir)
	{
		alCheck(alSource3f(sourceId, AL_GAIN, dir.x, dir.y, dir.z));
	}

	void AudioSource::setVel(const glm::vec3& vel)
	{
		alCheck(alSource3f(sourceId, AL_GAIN, vel.x, vel.y, vel.z));
	}

	u32 AudioSource::getId()
	{
		return sourceId;
	}
}
