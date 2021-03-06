#include <objects/Model.h>

namespace Skip {

    Model::Model()
        : SkipObject(), _modelPath(DEFAULT_MODEL) {

    }

    Model::Model(std::string name, glm::vec3 position, std::string texturePath, bool useIndexBuffer)
        : SkipObject(name, position, texturePath, useIndexBuffer), _modelPath(DEFAULT_MODEL) {
    }
    Model::Model(glm::vec3 position, std::string texturePath, bool useIndexBuffer) 
        : SkipObject(DEFAULT_MODEL_NAME, position, texturePath, useIndexBuffer) {

    }
    Model::Model(std::string name, glm::vec3 position, std::string texturePath, std::string modelPath, bool useIndexBuffer)
        : SkipObject(name, position, texturePath, useIndexBuffer) {
        _modelPath = modelPath;
    }
    Model::Model(glm::vec3 position, std::string texturePath, std::string modelPath, bool useIndexBuffer)
        : SkipObject(DEFAULT_MODEL_NAME, position, texturePath, useIndexBuffer) {
        _modelPath = modelPath;

    }
    Model::~Model() {
    }

    void Model::loadObject(float aspect) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _modelPath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        // attrib container holds pos, norms and tex coords
        // shapes container has all seperate objects and faces
        // in our case, we are ignoring material/texture per face

        //combine all faces in the file into a single model
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                // obj format assumes coord system where a vertical (y) coordinate of 0
                // means bottom of the image
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };
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
        }

        glm::mat4 pMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
        pMat[1][1] *= -1;

        _mvpUBO.proj = pMat;
        if (!_inheritLighting) {
            _lightUBO.position = _position;
        }
    }

}
