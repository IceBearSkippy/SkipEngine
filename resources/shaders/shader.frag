#version 450
#extension GL_ARB_seperate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 varyingLightDir;
layout(location = 3) in vec3 varyingVertPos;

layout(binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform PositionalLight {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
} posLight;

layout(location = 0) out vec4 outColor;

void main() {
	//outColor = vec4(fragTexCoord, 0.0, 1.0); // useful for debugging texture placement
	//outColor = texture(texSampler, fragTexCoord);
	vec4 texel = texture(texSampler, fragTexCoord);
	vec4 lightedColor = texel * posLight.diffuse * posLight.specular;
	outColor = vec4(lightedColor.xyz, 1.0);
	//outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, 1.0);
}