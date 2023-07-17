#pragma once

#include "ComponentManager.h"

#include "kuai/Core/Core.h"

#include "kuai/Renderer/Mesh.h"
#include "kuai/Renderer/Model.h"
#include "kuai/Renderer/Material.h"
#include "kuai/Renderer/Framebuffer.h"
#include "kuai/Renderer/Camera.h"

#include "kuai/Sound/AudioClip.h"

#include "kuai/Events/Event.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace kuai {
	// Forward Declaration
	class Transform;

	class Component
	{
	public:
		Component() = default;

		template<typename T>
		bool hasComponent() { return cm->hasComponent<T>(id); }

		template<typename T>
		T& getComponent() { return cm->getComponent<T>(id); }

		Transform& getTransform() { return cm->getComponent<Transform>(id); }

	private:
		ComponentManager* cm;
		EntityID id;

		friend ComponentManager;
	};

	class Name : public Component
	{

	};

	/** \class Transform
	*	\brief Describes 3D world position, rotation and scale of an object.
	*/
	class Transform : public Component
	{
	public:
		Transform() = default;
		Transform(const glm::vec3& pos) : pos(pos) {}

		glm::vec3 getPos() const;
		void setPos(const glm::vec3& pos);
		void setPos(float x, float y, float z);

		void translate(const glm::vec3& amount);
		void translate(float x, float y, float z);

		glm::vec3 getRot() const;
		void setRot(const glm::vec3& rot);
		void setRot(float x, float y, float z);

		void rotate(const glm::vec3& amount);
		void rotate(float x, float y, float z);

		glm::vec3 getScale() const;
		void setScale(const glm::vec3& scale);
		void setScale(float x, float y, float z);

		glm::vec3 getUp() const;
		glm::vec3 getRight() const;
		glm::vec3 getForward() const;

		glm::mat4 getModelMatrix() const;

	private:
		void updateComponents();

		void calcModelMatrix();

		glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rot = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 modelMatrix = glm::mat4(1.0f);
	};

	class Rigidbody2D : public Component
	{
	public:
		Rigidbody2D() = default;

		bool fixedRotation = false;

		float mass = 1.0f;
		float drag = 0.0f;

		glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

		bool useGravity = false;
	};

	class BoxCollider2D : public Component
	{
	public:
		BoxCollider2D() = default;

		glm::vec2 getSize() const;
		void setSize(float x, float y);

		glm::vec2 getOffset() const;
		void setOffset(float x, float y);

		float getRestitution() const;
		void setSize(float restitution);

		float getFriction() const;
		void setFriction(float friction);

	private:
		glm::vec2 size = { 0.5f, 0.5f };
		glm::vec2 offset = { 0.0f, 0.0f };

		// TODO: should be properties of a physics material
		float restitution = 0.5f;
		float friction = 0.5f;
	};

	/** \class MeshRenderer
	*	\brief Renders models or meshes to the screen.
	*/
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer() = default;
		MeshRenderer(Rc<Model> model) : model(model) {}
		MeshRenderer(Rc<Mesh> mesh) : model(makeRc<Model>(mesh)) {}

		void render()
		{

		}

		Rc<Model> getModel() { return model; }
		void setModel(Rc<Model> model) { this->model = model; }

		bool castsShadows() { return shadows; }
		void setShadows(bool shadows) { this->shadows = shadows; }

	private:
		Rc<Model> model;

		bool shadows = true;
	};

	class SpriteRenderer : public Component
	{
	public:
		SpriteRenderer()
		{

		}

	private:

	};

	/** \class Camera
	*	\brief Device through which the user views the world.
	*/
	class Cam : public Camera, public Component
	{
	public:
		Cam(float fov, float aspect, float zNear, float zFar) 
			: Camera(fov, aspect, zNear, zFar) {}

		bool isMain = false; // Indicates whether this is the main camera (i.e. the camera that renders to the window)
	};

	/** \class Light
	*	\brief Illuminates a scene. There are three types of light: directional, point and spot.
			   Directional lights only shine in one direction; they are used to model far away light sources such as the sun.
			   Point lights create an area of lighting.
			   Spotlights shine a concentrated beam of light with a provided angle and direction.
	*/
	class Light : public Component
	{
	public:
		enum class LightType
		{
			Directional = 0,
			Point = 1,
			Spot = 2
		};

		Light() = default;

		LightType getType() const;
		void setType(LightType type);

		glm::vec3 getCol() const;
		void setCol(const glm::vec3& col);
		void setCol(float x, float y, float z);

		float getIntensity() const;
		void setIntensity(float intensity);

		float getLinear() const;
		float getQuadratic() const;
		void setAttenuation(float linear, float quadratic);

		float getAngle() const;
		void setAngle(float angle);

	private:
		LightType type = LightType::Point;

		glm::vec3 col = { 1.0f, 1.0f, 1.0f };
		float intensity = 1;

		// Only used for point light and spot light (attenuation values)
		float linear = 0.1f;
		float quadratic = 0.025f;

		// Only used for spot light
		float angle = 30;

		friend class Transform;
	};


	/** \class Listener
	*	\brief Acts like a microphone; plays back sounds in a scene. Only one Listener is permitted per scene.
	*/
	class Listener : public Component
	{
	public:
		Listener() = default;

		float getGain();
		void setGain(float gain);

	private:
		void update();

		friend class Transform;
	};

	// Forward declaration
	class AudioSource;

	/** \class SoundSource
	*	\brief Acts like a speaker; generates sounds in a scene. Must be provided with an AudioClip to play.
	*/
	class SoundSource : public Component
	{
	public:
		SoundSource(bool stream = false);
		SoundSource(const SoundSource&) = delete;
		~SoundSource();

		void play();
		void pause();
		void stop();

		void setAudioClip(Rc<AudioClip> audioClip);

		float getPitch() const;
		void setPitch(float pitch);

		float getGain() const;
		void setGain(float gain);

		float getRolloff() const;
		void setRolloff(float rolloff);

		float getRefDist() const;
		void setRefDist(float refDist);

		bool isLoop() const;
		void setLoop(bool loop);

	private:
		void update();

		AudioSource* source;

		friend class Transform;
	};
}
