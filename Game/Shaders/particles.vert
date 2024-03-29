#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in mat4 model;
layout (location = 7) in vec4 color;

uniform mat4 projection;

out vec4 ourColor;

void main()
{
    ourColor = color;
    gl_Position = projection * model * vec4(position.xyz, 1.0);
}
