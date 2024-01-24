#ifndef EntityManager_H
#define EntityManager_H

#pragma once

#include "Base.h"
#include "BoundingBox.h"
#include "Constants.h"

constexpr int MAX_ENTITIES = 1024;

struct GameEntity {
    ID id;
    EntityType type;
    bool is_dirty;

    union {
        Rectangle rect;
        BoundingBox box;
        Oblast oblast;
    };
};

class EntityManager {
public:
    GameEntity entities[MAX_ENTITIES] = {};
    int ROLLING_INDEX = 0;

    void Update();

    ID CreateOblast(Oblast oblast);

    GameEntity* GetEntityById(ID id);
};

extern EntityManager E;

#endif
