#version 420 core

in vec2 ourTextureCoords;
in vec4 ourColor;

out vec4 color;

uniform sampler2D imageSampler;
uniform bool hideAlpha;

void main()
{
    if(ourTextureCoords.x < 0.0) {
        color = ourColor;
    }
    else {
        vec4 texColor = texture(imageSampler, ourTextureCoords);
        if(hideAlpha && texColor.a < 0.1)
         discard;

        color = texColor * ourColor;
    }
}  
