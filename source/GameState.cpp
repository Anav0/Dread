#include "GameState.h"
#include "EntityManager.h"

#include <algorithm>

GameState STATE;

void ChangeControl(f32 by)
{
    for (auto& e : E.entities) {
        if (e.type == EntityType::Oblast) {
            e.oblast.ukrainian_control += by;
            e.oblast.ukrainian_control = std::clamp(e.oblast.ukrainian_control, 0.0f, 1.0f);
            e.oblast.UpdateColorBasedOnControl();
        }
    }
}

v2 GetScreenSize()
{
    return STATE.window.screen_size;
}
