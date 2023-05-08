cmake_minimum_required(VERSION 3.16)

project(kuai VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(BUILD_SHARED_LIBS OFF)
set(LIBTYPE STATIC) # Sets OpenAL build type to static

add_library(${PROJECT_NAME}
    src/kpch.h
    src/kpch.cpp
    src/kuai.h

    src/kuai/Components/ComponentManager.h
    src/kuai/Components/Components.h
    src/kuai/Components/Components.cpp
    src/kuai/Components/Entity.h
    src/kuai/Components/EntityComponentSystem.h
    src/kuai/Components/EntityManager.h
    src/kuai/Components/System.h
    src/kuai/Components/System.cpp
    src/kuai/Components/SystemManager.h

    src/kuai/Core/App.h
    src/kuai/Core/App.cpp
    src/kuai/Core/Core.h
    src/kuai/Core/Input.h
    src/kuai/Core/KeyCodes.h
    src/kuai/Core/Layer.h
    src/kuai/Core/Layer.cpp
    src/kuai/Core/LayerStack.h
    src/kuai/Core/LayerStack.cpp
    src/kuai/Core/Log.h
    src/kuai/Core/Log.cpp
    src/kuai/Core/MouseBtnCodes.h
    src/kuai/Core/Timer.h
    src/kuai/Core/Window.h

    src/kuai/Events/AppEvent.h
    src/kuai/Events/Event.h
    src/kuai/Events/KeyEvent.h
    src/kuai/Events/MouseEvent.h

    src/kuai/Platform/Windows/WinInput.cpp
    src/kuai/Platform/Windows/WinWindow.h
    src/kuai/Platform/Windows/WinWindow.cpp

    src/kuai/Renderer/Buffer.h
    src/kuai/Renderer/Buffer.cpp
    src/kuai/Renderer/Camera.h
    src/kuai/Renderer/Cubemap.h
    src/kuai/Renderer/Cubemap.cpp
    src/kuai/Renderer/Framebuffer.h
    src/kuai/Renderer/Framebuffer.cpp
    src/kuai/Renderer/Geometry.h
    src/kuai/Renderer/Geometry.cpp
    src/kuai/Renderer/Material.h
    src/kuai/Renderer/Material.cpp
    src/kuai/Renderer/Mesh.h
    src/kuai/Renderer/Mesh.cpp
    src/kuai/Renderer/Model.h
    src/kuai/Renderer/Model.cpp
    src/kuai/Renderer/Renderer.h
    src/kuai/Renderer/Renderer.cpp
    src/kuai/Renderer/Shader.h
    src/kuai/Renderer/Shader.cpp
    src/kuai/Renderer/Shader.h
    src/kuai/Renderer/Shader.cpp
    src/kuai/Renderer/Skybox.h
    src/kuai/Renderer/Skybox.cpp
    src/kuai/Renderer/StaticShader.h
    src/kuai/Renderer/StaticShader.cpp
    src/kuai/Renderer/Texture.h
    src/kuai/Renderer/Texture.cpp

    src/kuai/Sound/AudioClip.h
    src/kuai/Sound/AudioClip.cpp
    src/kuai/Sound/AudioManager.h
    src/kuai/Sound/AudioManager.cpp
    src/kuai/Sound/AudioSource.h
    src/kuai/Sound/AudioSource.cpp
    src/kuai/Sound/MusicSource.h
    src/kuai/Sound/MusicSource.cpp

    vendor/stb_image/stb_image.h
    vendor/stb_image/stb_image.cpp
)
find_package(OpenGL REQUIRED)
set(MY_LIBS
    glfw
    glad
    assimp
    sndfile
    OpenAL::OpenAL
    GL
)

if (WIN32)
    # Add dwmapi and windows multimedia for audio
    set(MY_LIBS ${MY_LIBS} dwmapi winmm)

    target_compile_definitions(${PROJECT_NAME} PUBLIC
        KU_PLATFORM_WINDOWS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        GLFW_INCLUDE_NONE
        _CRT_SECURE_NO_WARNINGS
    )
else()

endif()

target_precompile_headers(${PROJECT_NAME}
    PUBLIC src/kpch.h
)

add_subdirectory(vendor/GLFW)
add_subdirectory(vendor/Glad)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/openal-soft)
add_subdirectory(vendor/libsndfile)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${PROJECT_BINARY_DIR}"
    PUBLIC src
    PUBLIC vendor/GLFW/include
    PUBLIC vendor/Glad/include
    PUBLIC vendor/glm
    PUBLIC vendor/stb_image
    PUBLIC vendor/assimp/include
    PUBLIC vendor/openal-soft/include
    PUBLIC vendor/libsndfile/include
    PUBLIC vendor/spdlog/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${MY_LIBS})
