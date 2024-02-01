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

void ComputerPhase() {
}

void GoToNextPhase() {
	//TODO: Disable the UI
	ComputerPhase();
	GoToNextTurn();
}

void DrawEndScreen() {}

void GoToNextTurn() {
	if (STATE.current_turn + 1 == STATE.max_turn) {
		DrawEndScreen();
	}

	STATE.current_turn += 1;
	STATE.turn_changed = true;
}

v2 GetScreenSize()
{
    return STATE.window.screen_size;
}

std::string GetDateStr() {
	return std::format("{} {}", GetMonth(), GetYear());
}

const char* GetMonth() {
	u16 month_index = STATE.current_turn % 12;
	return MONTHS[month_index];
}

std::string GetYear() {
	return std::to_string(2022 + STATE.current_turn / 12);
}