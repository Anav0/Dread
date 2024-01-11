#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"
#include "Constants.h"

class Model;
class ModelInBuffer;

ModelInBuffer AddModel(Model& model, v3 position, v3 size, u32 buffer_index = 0);
ModelInBuffer RenderSphere(v3 position, v3 size, u32 buffer_index = 0);

#endif
