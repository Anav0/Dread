#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in mat4 model;
layout (location = 7) in vec4 colorA;
layout (location = 8) in vec4 colorB;
layout (location = 9) in vec4 colorC;

uniform mat4 projection;

out vec4 ourColorA;
out vec4 ourColorB;
out vec4 ourColorC;

void main()
{
    gl_Position = projection * model * vec4(position.xyz, 1.0);
	ourColorA = colorA;
	ourColorB = colorB;
    ourColorC = colorC;
}
