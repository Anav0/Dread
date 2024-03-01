#pragma once

#ifndef EntityManager_H
#define EntityManager_H

#include "Base.h"
#include "BoundingBox.h"
#include "Constants.h"
#include "Gui.h"

constexpr int MAX_ENTITIES = 1024;

struct GameEntity {
    ID id;
    EntityType type;
    bool is_dirty;

    union {
        BoundingBox box;
        Oblast oblast;
    };
};

class EntityManager {
public:
    GameEntity entities[MAX_ENTITIES] = {};
    int ROLLING_INDEX = 0;

    ID CreateOblast(Oblast oblast);

    void Update();
    GameEntity* GetEntityById(ID id);
};

extern EntityManager E;

#endif
