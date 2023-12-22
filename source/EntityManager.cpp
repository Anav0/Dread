#include "EntityManager.h"
#include "Renderer.h"

EntityManager E;

void EntityManager::Update()
{
    for (size_t i = 0; i < ROLLING_INDEX; i++)
    {
        auto e = &entities[i];

        switch (e->type)
        {
        case Rect:

            if (e->is_dirty)
            {
                R.render_groups[e->render_group_index].UpdateBufferSection(e->buffer_index, e->rect.transform.size, e->rect.transform.position, e->rect.transform.rotation);
                R.render_groups[e->render_group_index].UpdateColor(e->buffer_index, e->rect.color);
            }
            break;
        }

        e->is_dirty = false;
    }
}

Entities *EntityManager::GetEntityById(ID id)
{
    for (size_t i = 0; i < ROLLING_INDEX; i++)
    {
        Entities *e = &entities[i];
        if (e->id == id)
            return e;
    }

    return nullptr;
}