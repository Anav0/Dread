#pragma once

#include "Constants.h"
#include <glm/ext/matrix_transform.hpp>

inline void convertToOpenglCoordSystem(f64 x, f64 y, f64* ox, f64* oy, int maxWidth, int maxHeight)
{
    *ox = 2 * x / maxWidth - 1;
    *oy = -2 * y / maxHeight + 1;
}

inline bool isPointInRect(f32 x, f32 y, f32 w, f32 h, f32 p_x, f32 p_y)
{
    f32 max_x = x + w;
    f32 min_x = x;

    f32 max_y = y + h;
    f32 min_y = y;

    return p_x <= max_x && p_x >= min_x && p_y >= min_y && p_y <= max_y;
}

inline bool isPointInRect(v2 position, v2 size, f32 p_x, f32 p_y)
{
    return isPointInRect(position.x, position.y, size.x, size.y, p_x, p_y);
}

inline void Move(m4* model, const v2 position)
{
    *model = glm::translate(*model, v3(position, 0.0f));
}

inline void Rotate(m4* model, const f32 degs)
{
    *model = glm::rotate(*model, glm::radians(degs), v3(0.0f, 0.0f, 1.0f));
}

inline m4 GetTransformMatrix(v2 position, v2 size, float rotate = 0.0, v2 scale = v2(1.0, 1.0))
{
    m4 model = m4(1.0f);
    model = glm::translate(model, v3(position, 0.0f));
    model = glm::translate(model, v3(size.x, size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), v3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, v3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    auto finalSize = size * scale;
    model = glm::scale(model, v3(finalSize.x, finalSize.y, 1.0f));
    return model;
}

inline v4 lerp(v4 a, v4 b, f32 progress) {
    v4 output;
    output.x = std::lerp(a.x, b.x, progress);
    output.y = std::lerp(a.y, b.y, progress);
    output.z = std::lerp(a.z, b.z, progress);
    output.w = std::lerp(a.w, b.w, progress);
    return output;
}

inline m4 GetTransformMatrix(v3 position, v3 size, f32 rotate = 0.0, v3 scale = v3(1.0, 1.0, 1.0))
{
    m4 model = m4(1.0f);
    model = glm::translate(model, position);
    model = glm::translate(model, size);
    model = glm::rotate(model, glm::radians(rotate), v3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, v3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, scale);

    return model;
}

inline m4 GetTransformMatrix(v3 position, f32 rotate = 0.0, v3 scale = v3(1.0, 1.0, 1.0))
{
    m4 model = m4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotate), v3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    return model;
}

struct AtlasTextureInfo {
    v2 position, size, scale;
    const char* uniqueId;
};

inline void CenterChildInParentX(v2* parent_pos, const v2* parent_size, v2* child_pos,
    v2* child_size)
{
    child_pos->x = (parent_pos->x + parent_size->x / 2.0) - (child_size->x / 2.0);
}

inline void CenterChildInParentY(v2* parent_pos, const v2* parent_size, v2* child_pos,
    v2* child_size)
{
    child_pos->y = (parent_pos->y + parent_size->y / 2.0) - (child_size->y / 2.0);
}

inline void CenterChildInParent(v2* parent_pos, const v2* parent_size, v2* child_pos,
    v2* child_size)
{
    CenterChildInParentX(parent_pos, parent_size, child_pos, child_size);
    CenterChildInParentY(parent_pos, parent_size, child_pos, child_size);
}

inline void ScaleToWidth(v2* scale, f32 actual_width, f32 desired_width)
{
    scale->x = (desired_width / actual_width);
    scale->y = (desired_width / actual_width);
}
