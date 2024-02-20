#include "GameState.h"
#include "EntityManager.h"

#include <algorithm>

GameState STATE;

u32 GameState::GetDataSizeForLayout(std::string name) {
	if (name == "gradients")
		return render_data.gradient_rects_n;
	if (name == "quads")
		return render_data.texture_rects_n;
	if (name == "fonts")
		return render_data.font_rects_n;
}

void* GameState::GetDataForLayout(std::string name, u32& size) {
	if (name == "gradients") {
		if (render_data.gradient_rects_n == 0) {
			size = 0;
			return &render_data.gradient_rects;
		}

		size = render_data.gradient_rects_n * sizeof(render_data.gradient_rects[0]);
		return &render_data.gradient_rects;
	}
	if (name == "quads") {
		if (render_data.texture_rects_n == 0) {
			size = 0;
			return &render_data.texture_rects;
		}

		size = render_data.texture_rects_n * sizeof(render_data.texture_rects[0]);
		return &render_data.texture_rects;
	}

	if (name == "fonts") {
		if (render_data.font_rects_n == 0) {
			size = 0;
			return &render_data.font_rects;
		}

		size = render_data.font_rects_n * sizeof(render_data.font_rects[0]);
		return &render_data.font_rects;
	}

	return nullptr;
}

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

void ComputerPhase() {

}

void GoToNextPhase() {
	ComputerPhase();
	GoToNextTurn();
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

void GameState::Reset() {
	this->render_data.texture_rects_n  = 0;
	this->render_data.gradient_rects_n = 0;
	STATE.turn_changed = false;
}