#include "GameState.h"
#include "EntityManager.h"

#include <algorithm>

GameState STATE;

Oblast* GetOblast(OblastCode code) {
	for (GameEntity& e : E.entities) {
		if (e.type == EntityType::Oblast && e.oblast.code == code) return &e.oblast;
	}
	return nullptr;
}

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

void DeliverSupport();

void GoToNextPhase() {

	switch(STATE.phase) {
		case GamePhase::Inital:
			STATE.phase = GamePhase::Events;
			break;
		case GamePhase::Events:
			STATE.phase = GamePhase::AllocateAssets;
			break;
		case GamePhase::AllocateAssets:
			STATE.phase = GamePhase::CPU;
			break;
		case GamePhase::CPU:
			STATE.phase = GamePhase::Resolution;
			break;
		case GamePhase::Resolution:
			STATE.current_turn += 1;
			STATE.phase = GamePhase::Events;
			break;
	};

	DeliverSupport();

	STATE.turn_changed = true;
}

void DrawEndScreen() {}

std::vector<Delivery> GetOnlyDeliveredSystems(SupportPackage& package) {
	return package.delivery;
}

Delivery* GetDeliveryByType(WeaponSystemType type) {
	for (Delivery& d : STATE.weapons_in_reserve) {
		if (d.system.type == type) return &d;
	}
	return nullptr;
}

void PromiseSupport(SupportPackage package) {
	STATE.promised_support.push_back(package);
}

void DeliverSupport() {
	for (SupportPackage& package : STATE.promised_support) {
		if (package.fully_delivered) continue;
		if (package.delivered_on_turn == STATE.current_turn) {
			std::vector<Delivery> systems_delivered = GetOnlyDeliveredSystems(package);
			
			for (Delivery& delivery : systems_delivered) {
				Delivery* matching_delivery = GetDeliveryByType(delivery.system.type);
				if (matching_delivery == nullptr) {
					STATE.weapons_in_reserve.push_back(delivery);
				} else {
					matching_delivery->n += delivery.n;
				}
			}
			
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
	
}

void GetDateStr(std::string& date_str) {
	date_str = std::format("{} {}", GetMonth(), 2022 + STATE.current_turn / 12);
}

const char* GetMonth() {
	u16 month_index = STATE.current_turn % 12;
	return MONTHS[month_index];
}

std::string GetYear() {
	return std::to_string(2022 + STATE.current_turn / 12);
}

void GameState::Reset() {
	STATE.turn_changed = false;
}


