#include <objects/Cube.h>

namespace Skip {

    Cube::Cube()
        : SkipObject() {

    }

    Cube::Cube(std::string name, glm::vec3 position, std::string texturePath, bool useIndexBuffer)
        : SkipObject(name, position, texturePath, useIndexBuffer) {
    }

    Cube::Cube(glm::vec3 position, std::string texturePath, bool useIndexBuffer)
        : SkipObject(DEFAULT_CUBE_NAME, position, texturePath, useIndexBuffer) {
    }

    Cube::~Cube() {
    }

    void Cube::loadObject(float aspect) {
        for (auto vertex : CUBE_VERTICES) {
            if (_useIndexBuffer) {
                if (_uniqueVertices.count(vertex) == 0) {
                    _uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                    _vertices.push_back(vertex);
                }
                _indices.push_back(_uniqueVertices[vertex]);
            } else {
                _vertices.push_back(vertex);
            }
        }

        glm::mat4 pMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
        pMat[1][1] *= -1;

        _mvpUBO.proj = pMat;
    }
}