#version 420 core

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 pos;
layout (location = 4) in vec2 size;
layout (location = 5) in mat4 model;
layout (location = 9) in vec2 textureCoords;
layout (location = 10) in vec2 textureCoords1;
layout (location = 11) in vec2 textureCoords2;
layout (location = 12) in vec2 textureCoords3;
layout (location = 13) in vec4 color;

uniform mat4 projection;

out vec4 ourColor;
out vec2 ourTextureCoords;

void main()
{
    ourColor = color;

    gl_Position = projection * model * vec4(position.xyz, 1.0);

   if(gl_VertexID == 0) ourTextureCoords = textureCoords;
   if(gl_VertexID == 1) ourTextureCoords = textureCoords1;
   if(gl_VertexID == 2) ourTextureCoords = textureCoords2;
   if(gl_VertexID == 3) ourTextureCoords = textureCoords3;
}
