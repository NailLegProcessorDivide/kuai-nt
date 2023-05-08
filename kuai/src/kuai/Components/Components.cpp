#include "kpch.h"
#include "Components.h"

#include "Entity.h"

#include "kuai/Renderer/Renderer.h"

#include "kuai/Sound/AudioManager.h"
#include "kuai/Sound/AudioSource.h"
#include "kuai/Sound/MusicSource.h"

namespace kuai {
	u32 Light::lightCount = 0;

	void Transform::updateComponents()
	{	
		if (hasComponent<Cam>())
		{
			getComponent<Cam>().updateViewMatrix(pos, rot);
		}

		if (hasComponent<Light>())
		{
			//getComponent<Light>().shadowCam.updateViewMatrix(pos, rot);
			getComponent<Light>().calcLightSpaceMatrix();
			// Renderer::updateShadowMap(getComponent<Light>());
		}

		if (hasComponent<AudioListener>())
		{
			getComponent<AudioListener>().update();
		}

		if (hasComponent<AudioSourceComponent>())
		{
			getComponent<AudioSourceComponent>().update();
		}
	}

	float AudioListener::getGain() { return AudioManager::getGlobalGain(); }
	void AudioListener::setGain(float gain) { AudioManager::setGlobalGain(gain); }

	void AudioListener::update()
	{
		AudioManager::setPos(getTransform().getPos());
		AudioManager::setOrientation(getTransform().getForward(), getTransform().getUp());
	}

	AudioSourceComponent::AudioSourceComponent(bool stream)
	{
		source = AudioManager::createAudioSource(stream);
	}

	AudioSourceComponent::~AudioSourceComponent()
	{
		AudioManager::destroyAudioSource(source->getId());
	}

	void AudioSourceComponent::update()
	{
		source->setPos(getTransform().getPos());
		source->setDir(getTransform().getForward());
	}

	Rc<AudioSource> AudioSourceComponent::get()
	{
		return source;
	}
}