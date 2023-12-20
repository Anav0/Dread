#pragma once

#include "Camera.h"
#include "Constants.h"
#include "EntityManager.h"
#include "GameState.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <vector>

void to_ndc(v3* vector, float width, float height)
{
    vector->x = (2.0f * vector->x) / width - 1.0f;
    vector->y = 1.0f - (2.0f * vector->y) / height;
    vector->z = 1.0f;
}

v4 GetRayFromEyes(m4* projection)
{
    auto camera = STATE.window.camera;
    v2 mouse = v2(STATE.window.mouse_x, STATE.window.mouse_y);
    v2 screen_size = STATE.window.screen_size;

    m4 view_matrix = camera.GetViewMatrix();

    v3 mouse_vector = v3(STATE.window.mouse_x, STATE.window.mouse_y, 0);
    to_ndc(&mouse_vector, STATE.window.screen_size.x, STATE.window.screen_size.y);

    v4 ray_clip = v4(mouse_vector.x, mouse_vector.y, 1.0f, 1.0f);
    v4 eye = inverse(*projection) * ray_clip;
    eye = v4(eye.x, eye.y, -1.0, 1.0);

    auto ray_world = (inverse(view_matrix) * eye);
    //ray_world = normalize(ray_world);

    return ray_world;
}
