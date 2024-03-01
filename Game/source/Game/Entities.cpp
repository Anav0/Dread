#include "Entities.h"

void Oblast::UpdateColorBasedOnControl()
{
    v4 color = lerp(RUSSIAN_COLOR, UKRAINE_COLOR, ukrainian_control);
    mesh.ChangeColor(color);
}
