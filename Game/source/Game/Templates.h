#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "Fight.h";
#include <map>

using WeaponTypeByPercent = std::map<WeaponSystemGeneralType, u16>;

using enum WeaponSystemGeneralType;

static const std::map<UnitType, WeaponTypeByPercent> TEMPLATES = {
    {
        UnitType::Panzer,
        { 
            { IFV,       20 }, 
            { Tank,      40 }, 
            { Infantry,  600 },
            { APC,       10 },
            { Artillery, 1 },
            { Mortar,    10 },
            { MLRS,      10 },
            { Vehicle,   200 },
            { Drone,     100 },
            { Tank,      10 },
            { MANPADS,   10 },
            { ATGM,      10 }, 
        }
    },
    {
        UnitType::Airborn,
        { 
            { IFV,       20 }, 
            { Tank,      40 }, 
            { Infantry,  600},
            { APC,       10},
            { Artillery, 1},
            { Mortar,    10},
            { MLRS,      10},
            { Vehicle,   200},
            { Drone,     100},
            { Tank,      10},
            { MANPADS,   10},
            { ATGM,      10}, 
        }
    },
    {
        UnitType::Infantry,
        { 
            { IFV,       20 }, 
            { Tank,      40 }, 
            { Infantry,  600 },
            { APC,       10 },
            { Artillery, 1 },
            { Mortar,    10 },
            { MLRS,      10 },
            { Vehicle,   200 },
            { Drone,     100 },
            { Tank,      10 },
            { MANPADS,   10 },
            { ATGM,      10 }, 
        }
    },
     {
        UnitType::Mech,
        { 
            { IFV,       20 }, 
            { Tank,      40 }, 
            { Infantry,  600 },
            { APC,       10 },
            { Artillery, 1 },
            { Mortar,    10 },
            { MLRS,      10 },
            { Vehicle,   200 },
            { Drone,     100 },
            { Tank,      10 },
            { MANPADS,   10 },
            { ATGM,      10 }, 
        }
    },
     {
        UnitType::Static,
        { 
            { IFV,       20 }, 
            { Tank,      40 }, 
            { Infantry,  600 },
            { APC,       10 },
            { Artillery, 1 },
            { Mortar,    10 },
            { MLRS,      10 },
            { Vehicle,   200 },
            { Drone,     100 },
            { Tank,      10 },
            { MANPADS,   10 },
            { ATGM,      10 }, 
        }
    },


};

#endif
