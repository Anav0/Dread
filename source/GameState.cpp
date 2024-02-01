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

const char* GetMonth() {
	u16 month_index = STATE.current_turn % 12;
	return MONTHS[month_index];
}

std::string GetYear() {
	return std::to_string(2022 + STATE.current_turn / 12);
}