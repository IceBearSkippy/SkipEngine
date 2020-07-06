#pragma once
#include <objects/SkipObject.h>
#include <stdexcept>

const std::vector<Skip::Vertex> CUBE_VERTICES = {
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.875f, 0.50f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.875f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.875f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.0f)},
    Skip::Vertex {glm::vec3(1.f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.75f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.75f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.75f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.125f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.125f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.125f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.75f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.75f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.75f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f)}
};
namespace Skip {

    class Cube : public SkipObject
    {
    public:
        Cube();
        Cube(glm::vec3 position, std::string texturePath = DEFAULT_TEXTURE);
        ~Cube();

        void loadObject();
    private:

    };
}
