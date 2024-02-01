#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"
#include "Constants.h"

class Model;
class MeshInBuffer;
class TextInBuffer;

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color, f32 rotation, f32 scale);
std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color = GREY, f32 rotation = 0.0f, f32 scale = 1.0f);
TextInBuffer AddText(std::string text, v2 pos, v4 color = BLACK, u8 font_size = 48);
TextInBuffer AddText(u8 size, v2 pos, v4 color = BLACK, u8 font_size = 48);

void AddResources(u8 font_size);
void AddSupportingCountries(u8 font_size);
void AddMap();

#endif
