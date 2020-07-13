#pragma once
#include <objects/SkipObject.h>
#include <stdexcept>


namespace Skip {

    class Sphere : public SkipObject
    {
    public:
        Sphere();
        Sphere(glm::vec3 position, int precision = 48, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        ~Sphere();

        int _precision;

        void loadObject(float aspect);
    private:

    };
}
