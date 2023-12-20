#include "RenderHelpers.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "BoundingBox.h"
#include "Base.h"

ID AddBoundingBox(BoundingBox box, int render_group_index)
{
    // assert(render_group_index >= 0);
    // assert(render_group_index < R.NumberOfRenderGroups());

    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
    // E.entities[E.ROLLING_INDEX].render_group_index = render_group_index;
    E.entities[E.ROLLING_INDEX].type = eBoundingBox;
    E.entities[E.ROLLING_INDEX].box = box;

    // auto render_group = &R.render_groups[render_group_index];
    // E.entities[E.ROLLING_INDEX].buffer_index = render_group->buffer->AddRect(&rect);

    ID tmp = E.ROLLING_INDEX;
    E.ROLLING_INDEX++;
    return tmp;
}

void AddRect(Rectangle rect, int render_group_index)
{
    assert(render_group_index >= 0);
    assert(render_group_index < R.NumberOfRenderGroups());

    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
    E.entities[E.ROLLING_INDEX].render_group_index = render_group_index;
    E.entities[E.ROLLING_INDEX].type = Rect;
    E.entities[E.ROLLING_INDEX].rect = rect;

    auto render_group = &R.render_groups[render_group_index];
    E.entities[E.ROLLING_INDEX].buffer_index = render_group->buffer->AddRect(&rect);

    E.ROLLING_INDEX++;
}
