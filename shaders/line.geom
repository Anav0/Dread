#version 330

layout (points) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();

    //gl_Position = gl_in[0].gl_Position + vec4( 0.5, 0.5, 0.0, 0.0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

   EndPrimitive();
}