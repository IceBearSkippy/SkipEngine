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

const std::string DEFAULT_TEXTURE = "resources/defaults/blue_texture.png";

namespace Skip {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texCoord;
        glm::vec3 normal;
        glm::vec3 tangent;


        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
        bool operator==(const Vertex& other) const {
            return position == other.position && color == other.color && texCoord == other.texCoord
                && normal == other.normal; // normal might not be needed (?)
        }
    };

}

namespace std {
    template<> struct hash<Skip::Vertex> {
        size_t operator()(Skip::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Skip {
    struct MvpBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    struct LightBufferObject {
        alignas(16) glm::vec4 ambient;
        alignas(16) glm::vec4 diffuse;
        alignas(16) glm::vec4 specular;
        alignas(16) glm::vec3 position;
    };

    class SkipObject
    {
    public:
        SkipObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), std::string texturePath = DEFAULT_TEXTURE, bool useIndexBuffer = false);

        ~SkipObject();
        virtual void loadObject() = 0;

        glm::mat4 GetPositionMatrix();

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

        bool _useIndexBuffer;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;

        MvpBufferObject _mvpUBO{};
        LightBufferObject _lightUBO{};

        std::vector<VkBuffer> _uniformBuffers;
        std::vector<VkDeviceMemory> _uniformBuffersMemory;
    private:

    };

    float toRadians(float degrees);
    glm::mat4 buildCameraLocation(glm::vec3 camera, glm::vec3 uComp, glm::vec3 vComp, glm::vec3 nComp);

    glm::mat4 buildTranslate(float x, float y, float z);
    glm::mat4 buildRotateX(float rad);
    glm::mat4 buildRotateY(float rad);
    glm::mat4 buildRotateZ(float rad);
    glm::mat4 buildScale(float x, float y, float z);
}
