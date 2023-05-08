#pragma once

#include <memory>

#include "Framebuffer.h"

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

namespace kuai {
    class Camera
    {
    public:
        enum class ProjectionType
		{
			Perspective,
			Ortho
		};

		Camera(float width, float height, float zNear, float zFar, float scale)
		{ 
			setOrtho(width, height, zNear, zFar);
			updateViewMatrix(glm::vec3(0.0f), glm::vec3(0.0f));
		}

		Camera(float fov, float aspect, float zNear, float zFar)
		{
			setPerspective(fov, aspect, zNear, zFar);
			updateViewMatrix(glm::vec3(0.0f), glm::vec3(0.0f));
		}

		inline glm::mat4& getViewMatrix()  { return viewMatrix; }
		inline glm::mat4& getProjectionMatrix() { return projMatrix; }

		void setPerspective(float fov, float aspect, float zNear, float zFar)
		{
			projectionType = ProjectionType::Perspective;
			this->fov = glm::radians(fov);
			this->aspect = aspect;
			this->zNear = zNear;
			this->zFar = zFar;
			updateProjectionMatrix();
		}

		void setOrtho(float width, float height, float zNear, float zFar)
		{
			projectionType = ProjectionType::Ortho;
			this->width = width;
			this->height = height;
			this->orthoNear = zNear;
			this->orthoFar = zFar;
			updateProjectionMatrix();
		}

		void updateViewMatrix(glm::vec3 pos, glm::vec3 rot)
        {
	        viewMatrix = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(glm::quat(rot)); // Rotate then translate, aka TR
		    viewMatrix = glm::inverse(viewMatrix); // Calculate inverse to get correct operation, aka (TR)^-1 = R^-1T^-1
        }

		void setTarget(Rc<Framebuffer> target) { this->target = target; }
		Rc<Framebuffer> getTarget() { return target; }

	private:
		void updateProjectionMatrix()
        {
            if (projectionType == ProjectionType::Perspective)
            {
                projMatrix = glm::perspective(fov, aspect, zNear, zFar);
            }
            else
            {
				projMatrix = glm::ortho(
					-width * 0.5f, width * 0.5f, 
					-height * 0.5f, height * 0.5f,
					orthoNear, orthoFar
				);
            }
        }

	private:
		ProjectionType projectionType;

		// Perspective params
		float fov;
		float aspect;
		float zNear, zFar;

		// Ortho params
		float width, height;
		float orthoNear, orthoFar;

		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::mat4 viewProjMatrix;

		// Framebuffer this camera will render to; none is default framebuffer
		Rc<Framebuffer> target = nullptr;
    };
}