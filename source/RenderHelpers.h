#ifndef RenderHelpers_H
#define RenderHelpers_H

#pragma once

#include "Base.h"

class BoundingBox;
class Rectangle;

enum AddOption {
    DoNotRender,
    Render
};

ID AddBoundingBox(BoundingBox box, int render_group_index = 2);
void AddRect(Rectangle rect, int render_group_index = 0);

#endif
