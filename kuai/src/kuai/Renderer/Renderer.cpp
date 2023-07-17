#include "kpch.h"

#include "Renderer.h"
#include "Shader.h"

#include "glad/glad.h"

#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/string_cast.hpp"

namespace kuai {
    Box<Renderer::RenderData> Renderer::renderData = std::make_unique<Renderer::RenderData>();

    void Renderer::init()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_FRAMEBUFFER_SRGB); // TODO: IMPLEMENT THIS MANUALLY IN SHADER AND TEXTURES

        Shader::init();
    }

    void Renderer::cleanup()
    {
        Shader::cleanup();
    }

    void Renderer::setCamera(Camera& camera)
    {
        renderData->projMatrix = camera.getProjectionMatrix();
        renderData->viewMatrix = camera.getViewMatrix();
    }

    void Renderer::render(Shader& shader)
    {
        shader.bind();

        shader.setUniform("CamData", "projMatrix", &renderData->projMatrix, sizeof(glm::mat4));
        shader.setUniform("CamData", "viewMatrix", &renderData->viewMatrix, sizeof(glm::mat4));

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, shader.getCommandCount(), sizeof(IndirectCommand));
    }

    void Renderer::setViewport(u32 x, u32 y, u32 width, u32 height)
    {
        glViewport(x, y, width, height);
    }

    void Renderer::setClearCol(const glm::vec4& col)
    {
        glClearColor(col.r, col.g, col.b, col.a);
    }

    void Renderer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}


