#pragma once

#include "Card.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Buffers.h"

enum AddOption {
    DoNotRender,
    Render
};

void RenderCard(ID id, int render_group_index = 0)
{
    auto e = E.GetEntityById(id);

    if (e == nullptr) {
        printf("Unable to render entity that is not in EntityManager already!\n");
        return;
    }

    e->render_group_index = render_group_index;
    auto render_group = &R.render_groups[render_group_index];
    auto atlas = render_group->atlas;
    e->buffer_index = render_group->buffer->AddTexturedRect(e->card.card_texture, atlas, e->card.transform.position, e->card.transform.size);
}

ID AddCard(Card card, int render_group_index = 0, AddOption option = Render)
{
    if (option == Render)
        assert(render_group_index >= 0);

    assert(render_group_index < R.NumberOfRenderGroups());

    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
    E.entities[E.ROLLING_INDEX].render_group_index = render_group_index;
    E.entities[E.ROLLING_INDEX].type = GameCard;
    E.entities[E.ROLLING_INDEX].card = card;

    if (option == Render) {
        RenderCard(E.ROLLING_INDEX, render_group_index);
    }

    E.ROLLING_INDEX++;
    return E.ROLLING_INDEX - 1;
}

void AddMainButton(v2 pos, float scale = 1.0f, int render_group_index = 1)
{
    assert(render_group_index >= 0);
    assert(render_group_index <= R.NumberOfRenderGroups());

    auto render_group = &R.render_groups[render_group_index];

    auto atlas = render_group->atlas;

    render_group->buffer->AddTexturedRect(&FANCY_BTN_TEX_INFO, atlas, pos, FANCY_BTN_TEX_INFO.size * scale);
}

void AddRect(Rectangle rect, int render_group_index = 0)
{
    assert(render_group_index >= 0);
    assert(render_group_index < R.NumberOfRenderGroups());

    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
    E.entities[E.ROLLING_INDEX].render_group_index = render_group_index;
    E.entities[E.ROLLING_INDEX].type = Rect;
    E.entities[E.ROLLING_INDEX].rect = rect;

    auto render_group = &R.render_groups[render_group_index];
    E.entities[E.ROLLING_INDEX].buffer_index = render_group->buffer->AddRect(&rect);

    E.ROLLING_INDEX++;
}

void AddMainButton(v3 pos, RenderGroup* render_group, float scale = 1.0f)
{
    render_group->buffer->AddTexturedRect(&FANCY_BTN_TEX_INFO, render_group->atlas, pos, FANCY_BTN_TEX_INFO.size * scale);
}

void AddRect(v3 pos, int render_group_index = 3)
{
    auto render_group = &R.render_groups[render_group_index];
    render_group->buffer->AddTexturedRect(&FANCY_BTN_TEX_INFO, render_group->atlas, pos, FANCY_BTN_TEX_INFO.size);
}
