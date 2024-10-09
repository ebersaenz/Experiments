#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texCoords;

out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN; // Tangent-Bitangent-Normal matrix

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main(void)
{
    TexCoords = texCoords;

    // Fragment position in world space
    FragPos = vec3(modelMatrix * vec4(position, 1.0));
        
    vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}