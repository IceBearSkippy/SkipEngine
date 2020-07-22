#version 450
#extension GL_ARB_seperate_shader_objects : enable

layout(set = 0, binding = 0) uniform MvpBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 norm;
} mvp;

layout(set = 0, binding = 2) uniform LightBufferObject {
    vec4 globalAmbient;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 matAmbient;
    vec4 matDiffuse;
    vec4 matSpecular;
    float matShininess;

    vec3 position;
} light;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 vertNormal;
layout(location = 4) in vec3 vertTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 varyingLightDir;  // vector pointing to the light
layout(location = 3) out vec3 varyingVertPos;   // vertex position in eye space
layout(location = 4) out vec3 varyingHalfVector;
layout(location = 5) out vec3 varyingNormal;
layout(location = 6) out vec3 lightPos;

void main() {
    mat4 mvMatrix = mvp.view * mvp.model;
    fragColor = vertColor;
    fragTexCoord = texCoord;
    varyingVertPos = (mvMatrix * vec4(vertPosition, 1.0)).xyz;
    
    varyingLightDir = (mvMatrix * vec4(light.position, 1.0)).xyz - varyingVertPos;
    varyingHalfVector = (varyingLightDir + (-varyingVertPos)).xyz;
    varyingNormal = (mvp.norm * vec4(vertNormal, 1.0)).xyz;
    lightPos = light.position;

    gl_Position = mvp.proj * mvMatrix * vec4(vertPosition, 1.0);

}