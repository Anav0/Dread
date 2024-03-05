#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

flat out int EntityId;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int entity_id;

void main()
{
		EntityId  = entity_id;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
