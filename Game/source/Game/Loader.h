#include <map>

#include "Entities.h"
#include "Weather.h"

class GameState;
class ModifiersManager;

std::map<OblastCode, std::tuple<Weather, GroundCondition>> GetInitialConditions();
GameState LoadGame();
void AddModifiers(ModifiersManager&);
