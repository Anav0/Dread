#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "GameState.h"

#include "Engine/Gui.h"
#include "Engine/Base.h"
#include "Engine/Constants.h"

#include <vector>

class Model;
class MeshInBuffer;

std::vector<MeshInBuffer> AddModel(v3 position, v3 size, std::string model_name, v4 color, f32 rotation, f32 scale);
std::vector<MeshInBuffer> AddModel(v3 position, std::string model_name, v4 color = GREY, f32 rotation = 0.0f, f32 scale = 1.0f);
void AddMap();
void AddSupportingCountries();

//------------------------------------------------------------------------

void DrawUI(WindowManager*);
void DrawResources(WindowManager*);
void DrawSupportingCountries(WindowManager*);
void DrawDeliveriesUI(WindowManager*);
void DrawTurnUI(WindowManager*);
void DrawSupportPackage(SupportPackage& package);
void DrawOblastInfo();

#endif
