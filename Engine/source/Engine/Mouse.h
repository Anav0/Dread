#pragma once

#include "Constants.h"
#include "WindowManager.h"

class Ray;
class Camera;

struct MouseInfo {
    v2 pos;
    MouseButton type;
    MouseAction action;
};

void to_ndc(v3* vector, f32 width, f32 height);

Ray GetRayFromEyes(WindowManager& window, Camera& camera, m4* projection);
