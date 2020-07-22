#pragma once
#include <objects/SkipObject.h>
#include <Camera.h>
#include <vector>
namespace Skip {
    class SkipScene
    {
    public:
        SkipScene();
        SkipScene(glm::vec3 cameraPosition);
        SkipScene(Camera* camera);
        ~SkipScene();

        std::vector<SkipObject*> _objects;

        void loadScene(float aspect);

        // Used to dynamically change objects for events
        void addObject(SkipObject* skipObject, SkipObject* parent = nullptr, bool inheritLighting = true);
        void removeObject(std::string name); 

        Camera* _camera;
    private:

    };
}
