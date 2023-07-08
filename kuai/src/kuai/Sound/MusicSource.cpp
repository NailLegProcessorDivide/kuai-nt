// This is based on SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)

#include "kpch.h"

#include "MusicSource.h"
#include "AudioClip.h"
#include "AudioManager.h"

#include <mutex>

namespace kuai {
	MusicSource::MusicSource()
	{
	}

	void MusicSource::cleanup()
	{
		awaitThread();
		AudioSource::cleanup();
	}

	void MusicSource::play()
	{
		bool isStreamingT = false;
		PlaybackState threadStartStateT = PlaybackState::Stopped;

		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			isStreamingT = isStreaming;
			threadStartStateT = threadStartState;
		}

		if (isStreamingT && (threadStartStateT == PlaybackState::Paused)) // Resume
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			threadStartState = PlaybackState:: Playing;
			alCheck(alSourcePlay(sourceId));
			return;
		}
		else if (!isStreamingT && thread.joinable()) // Streaming thread finished, let it join so it can be restarted
		{
			stop();
		}

		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			isStreaming = true;
			threadStartState = PlaybackState::Playing;
		}

		thread = std::thread(&MusicSource::stream, this); // Start updating stream in separate thread
	}

	void MusicSource::pause()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		isStreaming = false;
	}

	void MusicSource::stop()
	{
		awaitThread();
	}

	void MusicSource::setAudioClip(Rc<AudioClip> audioClip)
	{
		this->audioClip = audioClip;
	}

	void MusicSource::setLoop(bool loop)
	{
		this->loop = loop;
	}

	MusicSource::PlaybackState MusicSource::getStatus() const
	{
	    PlaybackState state = AudioSource::getStatus();

        // To compensate for the lag between play() and alSourceplay()
        if (state == PlaybackState::Stopped)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex);

            if (isStreaming)
                state = threadStartState;
        }

        return state;
	}

	void MusicSource::stream()
	{
		bool requestStop = false;

		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			if (threadStartState == PlaybackState::Stopped) // Launching a thread in stopped state does nothing
			{
				isStreaming = false;
				return;
			}
		}
		
		// Fill and enqueue all available buffers
		for (size_t i = 0; (i < BUF_COUNT) && !requestStop; i++)
		{
			if (fillAndPushBuf(i))
				requestStop = true;
		}

		alCheck(alSourcePlay(sourceId));

		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			if (threadStartState == PlaybackState::Paused) // If thread was launched paused
				alCheck(alSourcePause(sourceId));
		}

		while (1)
		{
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				if (!isStreaming)
					break;
			}

			if (AudioSource::getStatus() == PlaybackState::Stopped) // Stream interrupted
			{
				if (requestStop)
				{
					// End streaming
					std::lock_guard<std::recursive_mutex> lock(mutex);
					isStreaming = false;
				}
				else
				{
					alCheck(alSourcePlay(sourceId)); // Try and continue
				}
			}

			// Get number of processed buffers (i.e. number that are ready for reuse)
			ALint processed = 0;
			alCheck(alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &processed));
			
			while (processed--)
			{
				// Pop the first unused buffer from the queue
				ALuint buf;
				alCheck(alSourceUnqueueBuffers(sourceId, 1, &buf));

				uint32_t bufNo;
				for (int i = 0; i < BUF_COUNT; i++)
				{
					if (buffers[i] == buf)
						bufNo = i;
				}

				if (!requestStop)
				{
					if (fillAndPushBuf(bufNo))
						requestStop = true;
				}
			}

			// Leave some time for other threads if stream still playing
			if (AudioSource::getStatus() != PlaybackState::Stopped)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		// Stop the playback
		alCheck(alSourceStop(sourceId));

		// Dequeue any buffer left in the queue
		// Get the number of buffers still in the queue
		ALint queued;
		alCheck(alGetSourcei(sourceId, AL_BUFFERS_QUEUED, &queued));

		// Dequeue them all
		ALuint buf;
		for (ALint i = 0; i < queued; ++i)
			alCheck(alSourceUnqueueBuffers(sourceId, 1, &buf));
	}

	bool MusicSource::fillAndPushBuf(uint32_t bufNo)
	{
		bool requestStop = false;

		// Load chunk of data from file
		short* data = new short[BUF_SIZE];
		uint64_t amount = audioClip->loadData(data, BUF_SIZE);

		if (!loop && amount < BUF_SIZE) // If not looping and reached end of file
			requestStop = true;

		// Fill buffer
		uint32_t buf = buffers[bufNo];
		alCheck(alBufferData(buf, audioClip->getFormat(), &data[0], amount * sizeof(short), audioClip->getSamplerate()));
	
		// Push it to queue
		alCheck(alSourceQueueBuffers(sourceId, 1, &buf));

		delete[] data;

		return requestStop;
	}

	void MusicSource::awaitThread()
	{
		// Request thread to join
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			isStreaming = false;
		}

		if (thread.joinable())
			thread.join();
	}
}