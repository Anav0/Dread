#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in vec4 pos_and_size;
layout (location = 4) in mat4 model;
layout (location = 8) in int gradient_type;
layout (location = 9) in vec2 middle;
layout (location = 10) in float radial_factor;
layout (location = 11) in vec4 radial_position_and_smoothing;
layout (location = 12) in vec4 colors[4];

uniform mat4 projection;

out vec4     ourColors[4];
out vec2     ourMiddle;
flat out int ourGradientType;
out vec4     ourRadialPosAndSmoothing;
out vec4     ourPosAndSize;
out float    ourRadialFactor;

void main()
{
  gl_Position = projection * model * vec4(position.xyz, 1.0);

	ourPosAndSize   = pos_and_size;
	ourGradientType = gradient_type;
	ourMiddle       = middle;
	ourRadialFactor = radial_factor;
	ourRadialPosAndSmoothing = radial_position_and_smoothing;
	ourColors = colors;
}
