#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords; //Vertex
layout (location = 3) in mat4 aModel;     //Instanced
layout (location = 7) in vec4 aColor;
layout (location = 8) in int aEntityId;

out vec2 TexCoords;
out vec4 Color;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    Color = aColor;
    gl_Position = projection * view * aModel * vec4(aPos, 1.0);
}
