#pragma once
#include <objects/SkipObject.h>
#include <tiny_obj_loader.h>
namespace Skip {

    class ModelObject : public SkipObject
    {
    public:
        ModelObject();
        ModelObject(glm::vec3 position, std::string texturePath = DEFAULT_TEXTURE);
        ModelObject(glm::vec3 position, std::string texturePath, std::string model_path);
        ~ModelObject();

        std::string _modelPath;

        void loadModel();
    private:

    };
}

