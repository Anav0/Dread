#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"
#include "Constants.h"

class Model;
class MeshInBuffer;
class LabelHandle;
class ButtonHandle;

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color, f32 rotation, f32 scale);
std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color = GREY, f32 rotation = 0.0f, f32 scale = 1.0f);

void AddResources(u8 font_size, LabelHandle& ui_popular_support, LabelHandle& ui_reserve);
void AddSupportingCountries(u8 font_size);
void AddTurnUI(LabelHandle& ui_date, ButtonHandle& ui_button_in_buffer);
void AddMap();

#endif
