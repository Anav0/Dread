#include "GameState.h"

GameState STATE;

Oblast* GetOblast(OblastCode code)
{
    int i = static_cast<int>(code);
    return &STATE.oblasts[i];
}
