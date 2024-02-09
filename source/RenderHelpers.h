#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"
#include "Constants.h"
#include "GameState.h"
#include "Gui.h"

#include <vector>

class Model;
class MeshInBuffer;

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color, f32 rotation, f32 scale);
std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color = GREY, f32 rotation = 0.0f, f32 scale = 1.0f);
void AddMap();
void AddSupportingCountries();

//------------------------------------------------------------------------

void DrawUI();
void DrawResources();
void DrawSupportingCountries();
void DrawDeliveriesUI();
void DrawTurnUI();
void DrawSupportPackage(SupportPackage& package);
void DrawOblastInfo();

#endif
