#include "Mouse.h"
#include "Collision.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

void to_ndc(v3* vector, f32 width, f32 height)
{
    vector->x = (2.0f * vector->x) / width - 1.0f;
    vector->y = 1.0f - (2.0f * vector->y) / height;
    vector->z = 1.0f;
}

Ray GetRayFromEyes(WindowManager& window, Camera& camera, m4* projection)
{
    v2 mouse = v2(window.mouse_x, window.mouse_y);
    v2 screen = window.screen_size;

    // To normalized device space
    f32 mouseX = mouse.x  / (screen.x * 0.5f) - 1.0f;
    f32 mouseY = mouse.y / (screen.y * 0.5f) - 1.0f;

    m4 view = camera.GetViewMatrix();

    m4 invVP = glm::inverse(*projection * view);
    v4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
    v4 worldPos = invVP * screenPos;

    Ray r;
    r.direction = glm::normalize(glm::vec3(worldPos));
    r.position = camera.position;

    return r;
}