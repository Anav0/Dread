#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"

class BoundingBox;
class Rectangle;
class Ray;

enum AddOption {
    DoNotRender,
    Render
};

ID AddBoundingBox(BoundingBox box, int render_group_index = 2);
void AddRect(Rectangle rect, int render_group_index = 0);
void RenderSphere(v3 position, v3 size);

#endif
