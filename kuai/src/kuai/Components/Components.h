#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "kuai/Core/Core.h"
#include "ComponentManager.h"

#include "kuai/Renderer/Mesh.h"
#include "kuai/Renderer/Model.h"
#include "kuai/Renderer/Material.h"
#include "kuai/Renderer/Framebuffer.h"
#include "kuai/Renderer/Camera.h"

#include "kuai/Sound/AudioClip.h"

#include "kuai/Events/Event.h"

namespace kuai {
	// Forward Declaration
	class Transform;

	class Component
	{
	public:
		Component() = default;

		template<typename T>
		bool hasComponent() { return cm->hasComponent<T>(id); };

		template<typename T>
		T& getComponent() { return cm->getComponent<T>(id); }

		Transform& getTransform() { return cm->getComponent<Transform>(id); }

	private:
		ComponentManager* cm; // :(
		EntityID id;

		friend ComponentManager;
	};

	/** \class Transform
	*	\brief Describes 3D world position, rotation and scale of an object.
	*/
	class Transform : public Component
	{
	public:
		Transform() = default;
		Transform(const glm::vec3& pos) : pos(pos) {}

		glm::vec3& getPos() { return pos; }
		void setPos(const glm::vec3& pos)
		{
			this->pos = pos;
			updateComponents();
			calcModelMatrix();
		}
		void setPos(float x, float y, float z) { setPos({ x, y, z }); }

		void translate(const glm::vec3& amount)
		{
			this->pos += amount;
			updateComponents();
			calcModelMatrix();
		}
		void translate(float x, float y, float z) { translate({ x, y, z }); }

		glm::vec3 getRot() { return glm::degrees(rot); }
		void setRot(const glm::vec3& rot)
		{
			this->rot = glm::radians(rot);
			updateComponents();
			calcModelMatrix();
		}
		void setRot(float x, float y, float z) { setRot({ x, y, z }); }

		void rotate(const glm::vec3& amount)
		{
			this->rot += glm::radians(amount);
			updateComponents();
			calcModelMatrix();
		}
		void rotate(float x, float y, float z) { rotate({ x, y, z }); }

		glm::vec3 getScale() { return scale; }
		void setScale(const glm::vec3& scale)
		{
			this->scale = scale;
			calcModelMatrix();
		}
		void setScale(float x, float y, float z) { setScale({ x, y, z }); }

		glm::vec3 getUp() { return glm::rotate(glm::quat(rot), glm::vec3(0.0f, 1.0f, 0.0f)); }
		glm::vec3 getRight() { return glm::rotate(glm::quat(rot), glm::vec3(1.0f, 0.0f, 0.0f)); }
		glm::vec3 getForward() { return glm::rotate(glm::quat(rot), glm::vec3(0.0f, 0.0f, -1.0f)); }

		glm::mat4 getModelMatrix() { return modelMatrix; }

	private:
		void updateComponents();

		void calcModelMatrix()
		{
			modelMatrix = glm::translate(glm::mat4(1.0f), pos) *
				glm::toMat4(glm::quat(rot)) *
				glm::scale(glm::mat4(1.0f), scale);
		}

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

		glm::vec2 getSize() { return size; }
		void setSize(float x, float y) { size = glm::vec2(x, y); }

		glm::vec2 getOffset() { return offset; }
		void setOffset(float x, float y) { offset = glm::vec2(x, y); }

		float getRestitution() { return restitution; }
		void setSize(float restitution) { this->restitution = restitution; }

		float getFriction() { return friction; }
		void setFriction(float friction) { this->friction = friction; }

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
		MeshRenderer(Rc<Mesh> mesh) : model(MakeRc<Model>(mesh)) {}

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

	/** \class Camera
	*	\brief Device through which the user views the world.
	*/
	class Cam : public Camera, public Component
	{
	public:


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
		Light() : lightId(lightCount++)
		{
			calcLightSpaceMatrix();
		}

		glm::vec3 getCol() { return col; }
		void setCol(const glm::vec3& col) { this->col = col; }
		void setCol(float x, float y, float z) { setCol({ x, y, z }); }

		float getIntensity() { return intensity; }
		void setIntensity(float intensity) { this->intensity = intensity; }

		float getLinear() { return linear; }
		float getQuadratic() { return quadratic; }
		void setAttenuation(float linear, float quadratic) { this->linear = linear; this->quadratic = quadratic; }

		float getAngle() { return angle; }
		void setAngle(float angle) { this->angle = angle; }

		void setShadows(bool enabled) { shadows = enabled; }
		bool castsShadows() { return shadows; }

		uint32_t getId() { return lightId; }

	public: // TODO: Should not be public, use friend class
		glm::mat4& getLightSpaceMatrix() { return lightSpaceMatrix; }
		void calcLightSpaceMatrix() { /*lightSpaceMatrix = shadowCam.getProjectionMatrix() * shadowCam.getViewMatrix(); */ }

	private:
		uint32_t lightId = 0;

		glm::vec3 col = { 1.0f, 1.0f, 1.0f };
		float intensity = 1;

		// Attenuation values
		float linear = 0.1f;
		float quadratic = 0.025f;

		float angle = 0.0f;

		bool shadows = false;
		glm::mat4 lightSpaceMatrix;

	private:
		static uint32_t lightCount;

		friend class Transform;
	};

	// Forward declarations
	class AudioSource;

	/** \class AudioListener
	*	\brief Acts like a microphone; plays back sounds in a scene. Only one AudioListener is permitted per scene.
	*/
	class AudioListener : public Component
	{
	public:
		AudioListener() {}

		float getGain();
		void setGain(float gain);

	private:
		void update();

		friend class Transform;
	};

	/** \class AudioSourceComponent
	*	\brief Acts like a speaker; generates sounds in a scene. Must be provided with an AudioClip to play.
	*/
	class AudioSourceComponent : public Component
	{
	public:
		AudioSourceComponent(bool stream = false);
		AudioSourceComponent(const AudioSourceComponent&) = delete;
		~AudioSourceComponent();

		Rc<AudioSource> get();

	private:
		void update();

		Rc<AudioSource> source;

		friend class Transform;
	};
}
