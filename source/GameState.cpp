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
	ComputerPhase();
	GoToNextTurn();
}

void DrawEndScreen() {}

std::vector<WeaponSystem> GetOnlyDeliveredSystems(SupportPackage& package) {
	return package.weapons;
}

void PromiseSupport(SupportPackage package) {
	STATE.promised_support.push_back(package);
}

void DeliverSupport() {
	for (SupportPackage& package : STATE.promised_support) {
		if (package.fully_delivered) continue;
		if (package.delivered_on_turn == STATE.current_turn) {
			std::vector<WeaponSystem> systems_delivered = GetOnlyDeliveredSystems(package);
			Append(STATE.weapons_in_reserve, systems_delivered);
			
			#ifdef DEBUG
			printf("Package delivered: {}\n", package.name);
			#endif

			package.fully_delivered = true;
		}
	}
}

Country& GetCountryByCode(CountryCode code) {
	for (Country& country : STATE.countries) {
		if (country.code == code) return country;
	}
	assert(false);
	return STATE.countries[0];
}

void GoToNextTurn() {
	STATE.current_turn += 1;

	DeliverSupport();

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