#include "RenderHelpers.h"
#include "Base.h"
#include "BoundingBox.h"
#include "EntityManager.h"
#include "Model.h"
#include "Renderer.h"

//TODO: for now meshes are not part of render group

//Returns ID of entity in entity manager
//ID AddMap(GameMap map)
//{
//    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
//    E.entities[E.ROLLING_INDEX].type = eGameMap;
//    //E.entities[E.ROLLING_INDEX].map = map;
//
//    ID tmp = E.ROLLING_INDEX;
//    E.ROLLING_INDEX++;
//    return tmp;
//}

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
