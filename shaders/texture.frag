#version 420 core

in vec2 ourTextureCoords;
in vec4 ourColor;

out vec4 color;

uniform sampler2D imageSampler;

void main()
{
    vec4 texColor = texture(imageSampler, ourTextureCoords);

        if(ourTextureCoords.x <= 0) {
            color = ourColor;
        }
        else {
            color = texColor * ourColor;
        }
}  
