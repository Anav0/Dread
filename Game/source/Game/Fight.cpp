#include "Fight.h"
#include "GameState.h"
#include <numeric>

BattleGroup FormBattleGroup(Unit& unit) {
	BattleGroup group;

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
					battle_groups.push_back(FormBattleGroup(unit));
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
					battle_groups.push_back(FormBattleGroup(unit));
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

void Fire(u32 distance_in_m, std::vector<BattleGroup>& fire, std::vector<BattleGroup>& target) {

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
		this->attacker_distance_in_meters -= 100;

		Fire(this->attacker_distance_in_meters, defender_battle_grup, attacker_battle_grup);
		Fire(this->attacker_distance_in_meters, attacker_battle_grup, defender_battle_grup);

	}


}

void TestFight() {
	Fight fight;
	fight.attacker_distance_in_meters = 6000;
	fight.phase = 0;
	fight.ua_units[0] = 0;
	//fight.ua_units[1] = 1;

	fight.ru_units[0] = 0;
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
