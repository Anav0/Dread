#pragma once

#include "Constants.h"

class Ray;

void to_ndc(v3* vector, f32 width, f32 height);

Ray GetRayFromEyes(m4* projection);
