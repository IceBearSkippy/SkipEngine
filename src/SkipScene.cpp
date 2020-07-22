#include <SkipScene.h>

namespace Skip {

    SkipScene::SkipScene() {
        _camera = &Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    SkipScene::~SkipScene() {
    }

    SkipScene::SkipScene(glm::vec3 cameraPosition) {
        _camera = new Camera(cameraPosition);
    }

    SkipScene::SkipScene(Camera* camera) {
        _camera = camera;
    }

    void SkipScene::loadScene(float aspect) {
        for (SkipObject* object : _objects) {
            object->loadObject(aspect);
        }
    }

    void SkipScene::addObject(SkipObject* skipObject, SkipObject* parent, bool inheritLighting) {
        if (parent != nullptr) {
            skipObject->_inheritLighting = true;
            parent->addChild(skipObject, inheritLighting);
        }
        _objects.push_back(skipObject);
    }

    void SkipScene::removeObject(std::string name) {
        _objects.erase(
            std::remove_if(
                _objects.begin(),
                _objects.end(),
                [name](SkipObject* const& obj) { return obj->_name == name; }
            ),
            _objects.end()
        );
    }

}