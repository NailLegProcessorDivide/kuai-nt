#pragma once

#include "glm/glm.hpp"

namespace kuai {
	// Forward Declaration
	class AudioClip;

	const u32 BUF_COUNT = 4;
	const u32 BUF_SIZE = 32768;

    class AudioSource
    {
    public:
		enum class PlaybackState
		{
			Stopped,
			Paused,
			Playing
		};

        AudioSource();
		virtual void cleanup();
       
		virtual void play();
		virtual void pause();
		virtual void stop();

		virtual void setAudioClip(Rc<AudioClip> audioClip);

		float getPitch() const;
		void setPitch(float pitch);

		float getGain() const;
		void setGain(float gain);

		float getRolloff() const;
		void setRolloff(float rolloff);

		float getRefDist() const;
		void setRefDist(float refDist);

		bool isLoop() const;
		virtual void setLoop(bool loop);

		virtual PlaybackState getStatus() const;

	private:
		void setPos(const glm::vec3& pos);
		void setDir(const glm::vec3& dir);
		void setVel(const glm::vec3& vel);

		u32 getId();

	protected:
		Rc<AudioClip> audioClip = nullptr;
		bool loop = false;

		u32 sourceId = 0;
		
		u32 buffers[BUF_COUNT];

		friend class AudioManager;
		friend class SoundSource;
    };
}