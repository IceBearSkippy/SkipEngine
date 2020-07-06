#include <objects/Cube.h>

namespace Skip {

    Cube::Cube()
        : SkipObject() {

    }

    Cube::Cube(glm::vec3 position, std::string texturePath)
        : SkipObject(position, texturePath) {
    }

    Cube::~Cube() {
    }

    void Cube::loadObject() {
        for (auto vertex : CUBE_VERTICES) {
            if (_uniqueVertices.count(vertex) == 0) {
                _uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                _vertices.push_back(vertex);
            }
            _indices.push_back(_uniqueVertices[vertex]);
        }
    }
}