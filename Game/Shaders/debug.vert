#version 330 core

layout (location = 0) in vec3 aPos;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Color = vec4(0.85, 0.5, 0.5, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}