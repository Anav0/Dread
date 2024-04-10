#include "Fight.h"
#include "GameState.h"
#include <numeric>
#include <random>
#include <algorithm>

BattleGroup FormBattleGroup(u32 unit_index, Unit& unit) {
	BattleGroup group;

	group.name = "Battlegroup from: " + unit.name;
	group.unit_index = unit_index;

	u32 index = 0;
	for(WeaponSystem& weapon : unit.weapons) {
			group.weapons.push_back(weapon);
			u32 n = unit.weapons_counter[index] * 0.3;
			unit.weapons_counter[index] -= n;
			group.weapons_counter.push_back(n);
			group.morale.push_back(unit.morale[index]);
			index++;
	}

	return group;
}

std::vector<BattleGroup> Fight::FormRUGroup(UnitStance stance) {
	std::vector<BattleGroup> battle_groups;
	for(u8 i = 0; i < MAX_UNITS; i++) {
		UnitStance ru_unit_stance = this->ru_stance[i];	
		if(ru_unit_stance == stance) {
				i32 ru_unit_index = this->ru_units[i];	
				if(ru_unit_index != -1) {
					Unit& unit = STATE.troops_deployment.ru_units.at(ru_unit_index);
					battle_groups.push_back(FormBattleGroup(ru_unit_index, unit));
				}
		}
	}
	return battle_groups;
}

std::vector<BattleGroup> Fight::FormUAGroup(UnitStance stance) {
	std::vector<BattleGroup> battle_groups;
	for(u8 i = 0; i < MAX_UNITS; i++) {
		UnitStance ua_unit_stance = this->ua_stance[i];	
		if(ua_unit_stance == stance) {
				i32 ua_unit_index = this->ua_units[i];	
				if(ua_unit_index != -1) {
					Unit& unit = STATE.troops_deployment.ukr_units.at(ua_unit_index);
					battle_groups.push_back(FormBattleGroup(ua_unit_index ,unit));
				}
		}
	}
	return battle_groups;
}

bool MoralBroke(std::vector<BattleGroup>& groups) {
	f32 morale_threshold = 0.5;
	for(BattleGroup& group : groups) {
		f32 avg_morale = std::reduce(group.morale.begin(), group.morale.end()) / group.morale.size();
		if(avg_morale > morale_threshold) return true;
	}

	return true;

};

bool AttackerSufferedHeavyLosses(std::vector<BattleGroup>& group) {
	return false;
};

bool TryToHitTarget(u32 distance, Device& device) {
    return false;
    /*
	u32 index = 0;
	Accuracy acc = device.accuracy[0];
	while(distance < acc.range_in_meters) {
		index++;
            if (index > device.accuracy.size()-1) break;
		acc = device.accuracy[index];
	}

	std::random_device rd;
	std::uniform_real_distribution<f32> distribution(0.0, 1.0);
	std::mt19937 engine(rd());

	auto value = distribution(engine);

	return value < acc.change_to_hit;
	*/
}

void Fire(u32 distance_in_m, std::vector<BattleGroup>& fire, std::vector<BattleGroup>& target) {

	std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<u32> dist(0, target.size()-1);
  std::uniform_int_distribution<u32> d6(0, 6);

	for(BattleGroup& group : fire) {
		for(WeaponSystem& weapon : group.weapons) {
			for(u32 device_index : weapon.devices) {
				//if(device.accuracy[0].range_in_meters < distance_in_m) continue;

				//Pick target
				u32 index = dist(mt);
				BattleGroup& target_group = target.at(index);

				std::uniform_int_distribution<u32> dist(0, target_group.weapons.size()-1);

				index = dist(mt);
				WeaponSystem& target_system = target_group.weapons[index];

				/*
        printf("%s from %s fires at: %s ", device.name.c_str(), group.name.c_str(), target_system.name.c_str());

				//Hit
				if(TryToHitTarget(distance_in_m, device) && target_system.state > 0.0) {
				   printf("[HIT] ");
				//Record damage
				if(target_system.armor == Armor::Soft) {
          printf("SOFT: %f -= %i! ", target_system.state, device.soft);
					target_system.state -= device.soft;
					target_group.morale.at(index) -= 0.1;
					
				} else {
          printf("HARD: %f -= %i! ", target_system.state, device.hard);
					target_system.state -= device.hard;
					target_group.morale.at(index) -= 0.1;
				}
          printf("State: %f, Morale: %f\n", target_system.state, target_group.morale.at(index));
				} else {
					printf("[MISS]\n");
				}
				*/
			}
		}
	}

}

void Fight::SimulateAttack() {
	//TODO: initiative system for determining defenders and attackers
	Side attacking_side = Side::UA;
	Side defending_side = Side::RU;

	std::vector<BattleGroup> attacker_battle_grup = FormRUGroup(UnitStance::Committed);
	std::vector<BattleGroup> defender_battle_grup = FormUAGroup(UnitStance::Defending);

	while(MoralBroke(defender_battle_grup) ||
			  MoralBroke(attacker_battle_grup) ||
				AttackerSufferedHeavyLosses(attacker_battle_grup)) {

		//Attacking groups go forward!
		if (this->attacker_distance_in_meters > 0) {
			this->attacker_distance_in_meters -= 100;
                    if (this->attacker_distance_in_meters < 0)
                        this->attacker_distance_in_meters = 0;
		}

		Fire(this->attacker_distance_in_meters, defender_battle_grup, attacker_battle_grup);
		Fire(this->attacker_distance_in_meters, attacker_battle_grup, defender_battle_grup);

	this->phase++;
                printf("Phase: %i\n", this->phase);
                printf("Distance: %im\n", this->attacker_distance_in_meters);
	}


}

void TestFight() {
	Fight fight;
	fight.attacker_distance_in_meters = 6000;
	fight.phase = 0;
	fight.ua_units[0] = 0;
	fight.ua_stance[0] = UnitStance::Defending;
	//fight.ua_units[1] = 1;

	fight.ru_units[0] = 0;
	fight.ru_stance[0] = UnitStance::Committed;
	fight.ru_units[1] = 1;

	fight.SimulateAttack();
}

/*
 
Phases:

	0. During one turn attacker can perform n number of attacks, based on his determination
 0a. Each attack look like this:
	1. Attacker forms battlegroups
	2. Defender can fire at the gathering point if he has assets (aircraft, drones and artillery)
	3. Battlegroups then move towards defenders positions
	4. Assets on both sides fire if the range permits
		5a. If the attackers morale breakes he tries to retreat
		5b. If the defenders morale breakes they give away their positions
			5ba. Defender can counterattack to try to recapture lost positions
	6. 




 * */