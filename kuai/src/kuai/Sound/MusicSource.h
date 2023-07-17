// This is based on SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)

#pragma once

#include "AudioSource.h"

namespace kuai {
	// Forward Declaration
	class AudioClip;

    class MusicSource : public AudioSource
    {
    public:
        MusicSource();
		virtual void cleanup() override;
        
		virtual void play() override;
		virtual void pause() override;
		virtual void stop() override;

        virtual void setLoop(bool loop) override;

		virtual void setAudioClip(Rc<AudioClip> audioClip) override;

		virtual PlaybackState getStatus() const override;

	private:
		void stream();
		bool fillAndPushBuf(u32 bufNo);

		void awaitThread();
        
	private:
		std::thread thread;											// Streaming thread
		mutable std::recursive_mutex mutex;							// Thread mutex
		PlaybackState threadStartState{ PlaybackState::Stopped };	// Thread state when it starts
		bool isStreaming = false;
    };
}