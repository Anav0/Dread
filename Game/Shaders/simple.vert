#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aModel;
layout (location = 7) in vec4 aColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aModel * vec4(aPos, 1.0);
}
