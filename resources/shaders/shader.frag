#version 450
#extension GL_ARB_seperate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
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

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 varyingLightDir;
layout(location = 3) in vec3 varyingVertPos;
layout(location = 4) in vec3 varyingHalfVector;
layout(location = 5) in vec3 varyingNormal;
layout(location = 6) in vec3 lightPos;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(fragTexCoord, 0.0, 1.0); // useful for debugging texture placement
    vec4 texel = texture(texSampler, fragTexCoord);

    // normalize the light, normal and view vectors
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(-varyingVertPos);
    vec3 H = normalize(varyingHalfVector);

    float cosTheta = dot(L,N);
    float cosPhi = dot(H,N);
    float lightToVertDistance = pow(distance(varyingVertPos, lightPos), 2);
    vec3 ambient = ((light.globalAmbient * light.matAmbient) + (light.ambient * light.matAmbient)).xyz / lightToVertDistance;
    vec3 diffuse = (light.diffuse.xyz * light.matDiffuse.xyz * max(cosTheta, 0.0)) / lightToVertDistance;
    vec3 specular = light.specular.xyz * light.matSpecular.xyz * pow(max(cosPhi, 0.0), light.matShininess) / lightToVertDistance;

    outColor =  vec4((ambient + diffuse + specular), 1.0) * texel;
}
