#version 450
#extension GL_ARB_seperate_shader_objects : enable

layout(set = 0, binding = 0) uniform MvpBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} mvp;

layout(set = 0, binding = 2) uniform PositionalLight {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
} posLight;

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 vertNormal;
layout(location = 4) in vec3 vertTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 varyingLightDir;  // vector pointing to the light
layout(location = 3) out vec3 varyingVertPos;   // vertex position in eye space

void main() {
	mat4 mvMatrix = mvp.view * mvp.model;

	varyingVertPos = (mvMatrix * vec4(vertPosition, 1.0)).xyz;
	varyingLightDir = posLight.position - varyingVertPos;

	gl_Position = mvp.proj * mvMatrix * vec4(vertPosition, 1.0);

	fragColor = vertColor;
	fragTexCoord = texCoord;
}