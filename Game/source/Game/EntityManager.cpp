#include "EntityManager.h"
#include "Entities.h"

#include "Engine/Renderer.h"

EntityManager E;

ID EntityManager::CreateOblast(Oblast oblast)
{
    entities[ROLLING_INDEX].oblast = oblast;
    entities[ROLLING_INDEX].id     = ROLLING_INDEX;
    entities[ROLLING_INDEX].type   = EntityType::Oblast;

    return ROLLING_INDEX++;
}

void EntityManager::Update()
{
    for (size_t i = 0; i < ROLLING_INDEX; i++) {
        auto e = &entities[i];

        switch (e->type) {
        case EntityType::BoundingBox:
            break;
        case EntityType::Oblast:
            break;
        }

        e->is_dirty = false;
    }
}

GameEntity* EntityManager::GetEntityById(ID id)
{
    for (size_t i = 0; i < ROLLING_INDEX; i++) {
        GameEntity* e = &entities[i];
        if (e->id == id)
            return e;
    }

    return nullptr;
}
