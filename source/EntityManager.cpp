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