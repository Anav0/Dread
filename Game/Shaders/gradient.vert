#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 pos;
layout (location = 4) in vec2 size;
layout (location = 5) in mat4 model;
layout (location = 9) in int gradient_type;
layout (location = 10) in vec2 middle;
layout (location = 11) in float radial_factor;
layout (location = 12) in vec2 radial_position;
layout (location = 13) in vec2 smoothing;
layout (location = 14) in vec4 colors[4];

uniform mat4 projection;

out vec4     ourColors[4];
out vec2     ourMiddle;
flat out int ourGradientType;
out vec2     ourRadialPosition;
out vec2     ourSize;
out vec2     ourPosition;
out float    ourRadialFactor;
out vec2     ourSmoothing;

void main()
{
  gl_Position = projection * model * vec4(position.xyz, 1.0);

	ourColors = colors;
	ourMiddle = middle;
	ourSize = size;
	ourPosition = pos;
	ourSmoothing = smoothing;
	ourGradientType   = gradient_type;
	ourRadialPosition = radial_position;
	ourRadialFactor   = radial_factor;
}
