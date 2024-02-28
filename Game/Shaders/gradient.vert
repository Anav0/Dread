#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in mat4 model;
layout (location = 7) in int gradient_type;
layout (location = 8) in vec2 middle;
layout (location = 9) in float radial_factor;
layout (location = 10) in vec2 radial_position;
layout (location = 11) in vec4 colors[4];

uniform mat4 projection;

out vec4     ourColors[4];
out vec2     ourMiddle;
flat out int ourGradientType;
out vec2     ourRadialPosition;
out float    ourRadialFactor;

void main()
{
    gl_Position = projection * model * vec4(position.xyz, 1.0);

	ourColors = colors;
	ourMiddle = middle;
	ourGradientType   = gradient_type;
	ourRadialPosition = radial_position;
	ourRadialFactor   = radial_factor;
}
