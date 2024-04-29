#pragma once

struct Modifier {
    f32 defense_modifier = 1.25;
    f32 attack_modifier = 1.05;

    Modifier(f32 attack_modifier, f32 defense_modifier)
        : defense_modifier(defense_modifier)
        , attack_modifier(attack_modifier)
    {
    }
};
