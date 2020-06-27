#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <vulkan/vulkan.h>

// TODO: Add no_texture.png 
const std::string DEFAULT_TEXTURE = "defaults/no_texture.png";

namespace Skip {

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };
}

namespace std {
    template<> struct hash<Skip::Vertex> {
        size_t operator()(Skip::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Skip {
    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class SkipObject
    {
    public:
        SkipObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), std::string texturePath = DEFAULT_TEXTURE);

        ~SkipObject();
        virtual void loadModel() = 0;

        glm::vec3 _position;
        
        std::string _texturePath;
        VkImage _textureImage;
        VkDeviceMemory _textureImageMemory;
        VkImageView _textureImageView;
        VkSampler _textureSampler;
        uint32_t _mipLevels;
        std::vector<Vertex> _vertices;
        std::unordered_map<Vertex, uint32_t> _uniqueVertices;
        std::vector<uint32_t> _indices;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;

        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;
    private:

    };
}

