#include <objects/ModelObject.h>

namespace Skip {

    ModelObject::ModelObject() {

    }

    ModelObject::~ModelObject() {
    }

    ModelObject::loadModel() {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _skipObjects[i]->_modelPath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        // attrib container holds pos, norms and tex coords
        // shapes container has all seperate objects and faces
        // in our case, we are ignoring material/texture per face

        //combine all faces in the file into a single model
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                // obj format assumes coord system where a vertical (y) coordinate of 0 
                // means bottom of the image
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };
                if (_skipObjects[i]->_uniqueVertices.count(vertex) == 0) {
                    _skipObjects[i]->_uniqueVertices[vertex] = static_cast<uint32_t>(_skipObjects[i]->_vertices.size());
                    _skipObjects[i]->_vertices.push_back(vertex);
                }
                _skipObjects[i]->_indices.push_back(_skipObjects[i]->_uniqueVertices[vertex]);
            }
        }
    }

}

