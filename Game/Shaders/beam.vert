#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 size;
layout (location = 4) in mat4 model;
layout (location = 8) in vec4 color;

uniform mat4 projection;

out vec4 ourColor;
out vec2 ourSize;

void main()
{
    ourColor = color;
    ourSize = size;
    gl_Position = projection * model * vec4(position.xyz, 1.0);
}
