#pragma once
#include <objects/SkipObject.h>
#include <tiny_obj_loader.h>
#include <stdexcept>

const std::string DEFAULT_MODEL = "resources/defaults/box.obj";
const std::string DEFAULT_MODEL_NAME = "ModelSkipObject";

namespace Skip {

    class Model : public SkipObject
    {
    public:
        Model();
        Model(std::string name, glm::vec3 position, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        Model(glm::vec3 position, std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);
        Model(std::string name, glm::vec3 position, std::string texturePath, std::string modelPath, bool useIndexBuffer = false);
        Model(glm::vec3 position, std::string texturePath, std::string modelPath, bool useIndexBuffer = false);
        ~Model();

        std::string _modelPath;

        void loadObject(float aspect);
    private:

    };
}
