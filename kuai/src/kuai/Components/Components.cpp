#include "kpch.h"
#include "Components.h"

#include "Entity.h"

#include "kuai/Renderer/Renderer.h"

#include "kuai/Sound/AudioManager.h"
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

		if (hasComponent<Listener>())
		{
			getComponent<Listener>().update();
		}

		if (hasComponent<SoundSource>())
		{
			getComponent<SoundSource>().update();
		}
	}

	float Listener::getGain() { return AudioManager::getGlobalGain(); }
	void Listener::setGain(float gain) { AudioManager::setGlobalGain(gain); }

	void Listener::update()
	{
		AudioManager::setPos(getTransform().getPos());
		AudioManager::setOrientation(getTransform().getForward(), getTransform().getUp());
	}

	SoundSource::SoundSource(bool stream)
	{
		source = AudioManager::createAudioSource(stream);
	}

	SoundSource::~SoundSource()
	{
		AudioManager::destroyAudioSource(source->getId());
	}

	void SoundSource::update()
	{
		source->setPos(getTransform().getPos());
		source->setDir(getTransform().getForward());
	}
}