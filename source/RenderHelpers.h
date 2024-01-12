#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"
#include "Constants.h"

class Model;
class MeshInBuffer;

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color, f32 rotation, f32 scale);

#endif
