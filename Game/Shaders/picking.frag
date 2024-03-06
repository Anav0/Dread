#version 420 core

out vec4 FragColor;

flat in int EntityId;

void main()
{    
   FragColor = vec4(EntityId, EntityId, EntityId, 1.0);
}
