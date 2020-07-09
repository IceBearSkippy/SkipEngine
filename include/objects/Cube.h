#pragma once
#include <objects/SkipObject.h>
#include <stdexcept>

const std::vector<Skip::Vertex> CUBE_VERTICES = {

    // vertex, color, texCoord, normal

    // + y (faces up)
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f), glm::vec3(0.0f, 1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.875f, 0.50f), glm::vec3(0.0f, 1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.875f, 0.25f), glm::vec3(0.0f, 1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.875f, 0.25f), glm::vec3(0.0f, 1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f), glm::vec3(0.0f, 1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f), glm::vec3(0.0f, 1.0f, 0.0f)},
    
    // + z (faces towards screen)
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f), glm::vec3(0.0f, 0.0f, 1.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f), glm::vec3(0.0f, 0.0f, 1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f), glm::vec3(0.0f, 0.0f, 1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},

    // - x (left)
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.75f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.75f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.75f), glm::vec3(-1.0f, 0.0f, 0.0f)},

    // - y (bottom)
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.125f, 0.50f), glm::vec3(0.0f, -1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f), glm::vec3(0.0f, -1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f), glm::vec3(0.0f, -1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.125f, 0.50f), glm::vec3(0.0f, -1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f), glm::vec3(0.0f, -1.0f, 0.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.125f, 0.25f), glm::vec3(0.0f, -1.0f, 0.0f)},

    // + x (right)
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f), glm::vec3(1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f), glm::vec3(1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f), glm::vec3(1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f), glm::vec3(1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.25f), glm::vec3(1.0f, 0.0f, 0.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.25f), glm::vec3(1.0f, 0.0f, 0.0f)},
    
    // - z (faces away screen)
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.75f), glm::vec3(0.0f, 0.0f, -1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.75f), glm::vec3(0.0f, 0.0f, -1.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f), glm::vec3(0.0f, 0.0f, -1.0f)},
    Skip::Vertex {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.75f), glm::vec3(0.0f, 0.0f, -1.0f)},
    Skip::Vertex {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.625f, 0.50f), glm::vec3(0.0f, 0.0f, -1.0f)},
    Skip::Vertex {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.375f, 0.50f), glm::vec3(0.0f, 0.0f, -1.0f)}
};
namespace Skip {

    class Cube : public SkipObject
    {
    public:
        Cube();
        Cube(glm::vec3 position, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        ~Cube();

        void loadObject();
    private:

    };
}
