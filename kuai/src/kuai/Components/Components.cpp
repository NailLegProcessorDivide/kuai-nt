#include "kpch.h"
#include "Components.h"

#include "Entity.h"

#include "kuai/Renderer/Renderer.h"
#include "kuai/Renderer/Shader.h"

#include "kuai/Sound/AudioManager.h"
#include "kuai/Sound/AudioSource.h"

namespace kuai {
	glm::vec3 Transform::getPos() const
	{
		return pos;
	}

	void Transform::setPos(const glm::vec3& pos)
	{
		this->pos = pos;
		updateComponents();
		calcModelMatrix();
	}

	void Transform::setPos(float x, float y, float z)
	{
		setPos({ x, y, z });
	}

	void Transform::translate(const glm::vec3& amount)
	{
		this->pos += amount;
		updateComponents();
		calcModelMatrix();
	}

	void Transform::translate(float x, float y, float z)
	{
		translate({ x, y, z });
	}

	glm::vec3 Transform::getRot() const
	{
		return glm::degrees(rot);
	}

	void Transform::setRot(const glm::vec3& rot)
	{
		this->rot = glm::radians(rot);
		updateComponents();
		calcModelMatrix();
	}

	void Transform::setRot(float x, float y, float z)
	{
		setRot({ x, y, z });
	}

	void Transform::rotate(const glm::vec3& amount)
	{
		this->rot += glm::radians(amount);
		updateComponents();
		calcModelMatrix();
	}

	void Transform::rotate(float x, float y, float z)
	{
		rotate({ x, y, z });
	}

	glm::vec3 Transform::getScale() const
	{
		return scale;
	}

	void Transform::setScale(const glm::vec3& scale)
	{
		this->scale = scale;
		calcModelMatrix();
	}

	void Transform::setScale(float x, float y, float z)
	{
		setScale({ x, y, z });
	}

	glm::vec3 Transform::getUp() const
	{
		return glm::rotate(glm::quat(rot), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Transform::getRight() const
	{
		return glm::rotate(glm::quat(rot), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Transform::getForward() const
	{
		return glm::rotate(glm::quat(rot), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::mat4 Transform::getModelMatrix() const
	{
		return modelMatrix;
	}

	void Transform::updateComponents()
	{	
		if (hasComponent<Cam>())
		{
			getComponent<Cam>().updateViewMatrix(pos, rot);
		}

		if (hasComponent<Light>())
		{

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

	void Transform::calcModelMatrix()
	{
		modelMatrix = glm::translate(glm::mat4(1.0f), pos) *
			glm::toMat4(glm::quat(rot)) *
			glm::scale(glm::mat4(1.0f), scale);
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

	void SoundSource::play()
	{
		source->play();
	}

	void SoundSource::pause()
	{
		source->pause();
	}

	void SoundSource::stop()
	{
		source->stop();
	}

	void SoundSource::setAudioClip(Rc<AudioClip> audioClip)
	{
		source->setAudioClip(audioClip);
	}

	float SoundSource::getPitch() const
	{
		return source->getPitch();
	}

	void SoundSource::setPitch(float pitch)
	{
		source->setPitch(pitch);
	}

	float SoundSource::getGain() const
	{
		return source->getGain();
	}

	void SoundSource::setGain(float gain)
	{
		source->setGain(gain);
	}

	float SoundSource::getRolloff() const
	{
		return source->getRolloff();
	}

	void SoundSource::setRolloff(float rolloff)
	{
		source->setRolloff(rolloff);
	}

	float SoundSource::getRefDist() const
	{
		return source->getRefDist();
	}

	void SoundSource::setRefDist(float refDist)
	{
		source->setRefDist(refDist);
	}

	bool SoundSource::isLoop() const
	{
		return source->isLoop();
	}

	void SoundSource::setLoop(bool loop)
	{
		source->setLoop(loop);
	}

	void SoundSource::update()
	{
		source->setPos(getTransform().getPos());
		source->setDir(getTransform().getForward());
	}

	glm::vec2 BoxCollider2D::getSize() const
	{
		return size;
	}

	void BoxCollider2D::setSize(float x, float y)
	{
		size = glm::vec2(x, y);
	}

	glm::vec2 BoxCollider2D::getOffset() const
	{
		return offset;
	}

	void BoxCollider2D::setOffset(float x, float y)
	{
		offset = glm::vec2(x, y);
	}

	float BoxCollider2D::getRestitution() const
	{
		return restitution;
	}

	void BoxCollider2D::setSize(float restitution)
	{
		this->restitution = restitution;
	}

	float BoxCollider2D::getFriction() const
	{
		return friction;
	}

	void BoxCollider2D::setFriction(float friction)
	{
		this->friction = friction;
	}

	Light::LightType Light::getType() const
	{
		return type;
	}

	void Light::setType(LightType type)
	{
		this->type = type;
	}

	glm::vec3 Light::getCol() const
	{
		return col;
	}

	void Light::setCol(const glm::vec3& col)
	{
		this->col = col;
	}

	void Light::setCol(float x, float y, float z)
	{
		setCol({ x, y, z });
	}

	float Light::getIntensity() const
	{
		return intensity;
	}

	void Light::setIntensity(float intensity)
	{
		this->intensity = intensity;
	}

	float Light::getLinear() const
	{
		return linear;
	}

	float Light::getQuadratic() const
	{
		return quadratic;
	}

	void Light::setAttenuation(float linear, float quadratic)
	{
		this->linear = linear; 
		this->quadratic = quadratic;
	}

	float Light::getAngle() const
	{
		return angle;
	}

	void Light::setAngle(float angle)
	{
		this->angle = angle;
	}
}

