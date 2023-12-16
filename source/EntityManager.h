#ifndef EntityManager_H
#define EntityManager_H

#pragma once

#include "Constants.h"
#include "Base.h"

constexpr int MAX_ENTITIES = 1024;

struct Entities {
    ID id;
    EntityType type;
    int render_group_index, buffer_index;
    bool is_dirty;

    union {
        Rectangle rect;
    };
};

class EntityManager {
public:
    Entities entities[MAX_ENTITIES] = {};
    int ROLLING_INDEX = 0;

    void Update();

    Entities* GetEntityById(ID id)
    {
        for (size_t i = 0; i < ROLLING_INDEX; i++) {
            Entities* e = &entities[i];
            if (e->id == id)
                return e;
        }

        return nullptr;
    }
};

extern EntityManager E;

#endif
