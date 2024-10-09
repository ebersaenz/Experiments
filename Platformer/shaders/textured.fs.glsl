#version 450 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D diffuseSampler;
uniform sampler2D normalSampler;
uniform bool hasDiffuse;

void main(void)
{        
    if (hasDiffuse) {    
        color = texture(diffuseSampler, TexCoords);
    } else {
        color = vec4(0f, 1.0f, 0.0f, 1.0f);
    }
}