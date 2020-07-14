#pragma once
#include <objects/SkipObject.h>
#include <stdexcept>

const std::string DEFAULT_SPHERE_NAME = "SphereSkipObject";
namespace Skip {

    class Sphere : public SkipObject
    {
    public:
        Sphere();
        Sphere(std::string name = DEFAULT_SPHERE_NAME, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), int precision = 48, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        Sphere(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), int precision = 48, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        ~Sphere();

        int _precision;

        void loadObject(float aspect);
    private:

    };
}
