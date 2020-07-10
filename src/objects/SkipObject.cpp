#include <objects/SkipObject.h>

namespace Skip {


    SkipObject::SkipObject(glm::vec3 position, std::string texturePath, bool useIndexBuffer) {
        _position = position;
        _texturePath = texturePath;
        _useIndexBuffer = useIndexBuffer;
        _mvpUBO.model = GetPositionMatrix();
    }
    SkipObject::~SkipObject() {
    }

    glm::mat4 SkipObject::GetPositionMatrix() {
        return buildTranslate(_position.x, _position.y, _position.z);
    }

    VkVertexInputBindingDescription Vertex::getBindingDescription() {
        // manage attribute binding per vertex
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0; // specifies the index of the binding in the array of bindings
        bindingDescription.stride = sizeof(Vertex); // number of bytes from one entry to next
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        // we have two attributes: position and color (hence the size)
        attributeDescriptions[0].binding = 0; // binding the per-vertex data
        attributeDescriptions[0].location = 0; // location directive of the input in the vertex shader
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // type of data (look at reference guide)
        attributeDescriptions[0].offset = offsetof(Vertex, position); // specifies the number of bytes since the start of the per-vertex data

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }


    float toRadians(float degrees) {
        return (degrees * 2.0f * 3.14159f) / 360.0f;
    }

    glm::mat4 buildCameraLocation(glm::vec3 camera, glm::vec3 uComp, glm::vec3 vComp, glm::vec3 nComp) {
        glm::mat4 rotMat = glm::mat4(
            uComp.x, uComp.y, uComp.z, 0.0,
            vComp.x, vComp.y, vComp.z, 0.0,
            nComp.x, nComp.y, nComp.z, 0.0,
            0.0, 0.0, 0.0, 1.0
        );

        glm::mat4 camMat = buildTranslate(camera.x, camera.y, camera.z);
        return camMat * rotMat;
    }

    glm::mat4 buildTranslate(float x, float y, float z) {
        glm::mat4 trans = glm::mat4(
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            x, y, z, 1.0
        );
        return trans;
    }

    glm::mat4 buildRotateX(float rad) {
        glm::mat4 xrot = glm::mat4(
            1.0, 0.0, 0.0, 0.0,
            0.0, cos(rad), -sin(rad), 0.0,
            0.0, sin(rad), cos(rad), 0.0,
            0.0, 0.0, 0.0, 1.0
        );
        return xrot;
    }

    glm::mat4 buildRotateY(float rad) {
        glm::mat4 yrot = glm::mat4(
            cos(rad), 0.0, sin(rad), 0.0,
            0.0, 1.0, 0.0, 0.0,
            -sin(rad), 0.0, cos(rad), 0.0,
            0.0, 0.0, 0.0, 1.0
        );
        return yrot;
    }

    glm::mat4 buildRotateZ(float rad) {
        glm::mat4 zrot = glm::mat4(
            cos(rad), -sin(rad), 0.0, 0.0,
            sin(rad), cos(rad), 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        );
        return zrot;
    }

    glm::mat4 buildScale(float x, float y, float z) {
        glm::mat4 scale = glm::mat4(
            x, 0.0, 0.0, 0.0,
            0.0, y, 0.0, 0.0,
            0.0, 0.0, z, 0.0,
            0.0, 0.0, 0.0, 1.0
        );
        return scale;
    }
}

