#pragma once
#include <objects/SkipObject.h>
#include <tiny_obj_loader.h>
#include <stdexcept>

const std::string DEFAULT_MODEL = "resources/defaults/box.obj";

namespace Skip {

    class Model : public SkipObject
    {
    public:
        Model();
        Model(glm::vec3 position, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        Model(glm::vec3 position, std::string texturePath, std::string model_path, bool useIndexBuffer = false);
        ~Model();

        std::string _modelPath;

        void loadObject(float aspect);
    private:

    };
}
