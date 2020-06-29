#include <objects/SkipObject.h>

namespace Skip {


    SkipObject::SkipObject(glm::vec3 position, std::string texturePath ) {
        _position = position;
        _texturePath = texturePath;
    }
	SkipObject::~SkipObject() {
	}

    VkVertexInputBindingDescription Vertex::getBindingDescription() {
        // manage attribute binding per vertex
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0; // specifies the index of the binding in the array of bindings
        bindingDescription.stride = sizeof(Vertex); // number of bytes from one entry to next
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        // we have two attributes: position and color (hence the size)
        attributeDescriptions[0].binding = 0; // binding the per-vertex data
        attributeDescriptions[0].location = 0; // location directive of the input in the vertex shader
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // type of data (look at reference guide)
        attributeDescriptions[0].offset = offsetof(Vertex, pos); // specifies the number of bytes since the start of the per-vertex data

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

}

