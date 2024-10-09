#version 450 core

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;  

out vec4 color;

uniform sampler2D diffuseSampler;
uniform sampler2D normalSampler;
uniform bool isValidMaterial;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main(void)
{
    if (!isValidMaterial) {            
        color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    } 
    
    vec3 normal = texture(normalSampler, TexCoords).rgb;
    normal = normalize(TBN * (normal * 2.0 - 1.0));

    vec3 diffuseColor = texture(diffuseSampler, TexCoords).rgb;

    // Simple directional lighting
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(normal, lightDirNorm), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient lighting
    vec3 ambient = 0.1 * lightColor;

    // Combine results
    vec3 result = (ambient + diffuse) * diffuseColor;
    color = vec4(result, 1.0);
}