#pragma once

inline void FillBattleWithMocks(Battle* battle, ResourceManager* resourceManager)
{
    auto scale = glm::vec3(1);

    Transform card_transform = {};
    card_transform.size = glm::vec3(CARD_W, CARD_H, 0);
    card_transform.position = glm::vec3(50.0f, 50.0f, 0);
    card_transform.rotation = 0;

    // Create sample deck for each side
    for (size_t i = 0; i < 20; i++) {
        auto quality = i % 2 != 0 ? Gold : Bronze;
        Card card = *new Card(resourceManager, card_transform, Legendary, quality, "conscript", "nilfgaard");
        card.transform.scale = scale;
        card.base_points = i + 1;
        card.points = i + 1;
        card.supplies = i + 4;

        battle->side_a_deck.push_back(card);

        card = *new Card(resourceManager, card_transform, Common, quality, "crossbowmen", "northern");
        card.transform.scale = scale;
        card.base_points = i + 1;
        card.points = i + 1;
        card.supplies = i + 4;

        battle->side_b_deck.push_back(card);
    }
}