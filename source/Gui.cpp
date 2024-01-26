#include "Gui.h"
#include "EntityManager.h"
#include "Misc.h"
#include "RenderHelpers.h"
#include "Renderer.h"
#include "TextRenderer.h"

Gui UI;

void Gui::DrawBtn(const char* text, u8 font_size, v2 pos, void on_click())
{
    auto mouse_x = STATE.window.mouse_x;
    auto mouse_y = STATE.window.mouse_y;

    v2 text_size = TR.GetTextSize(text, font_size);

    u8 padding_x = 30;
    u8 padding_y = 20;

    Transform transform {};
    transform.size = v3(text_size.x + padding_x, text_size.y + padding_y, 0.0);
    transform.position = v3(pos.x, pos.y, 0.0);
    transform.rotation = 0.0;
    transform.scale = v3(1.0);

    Rectangle rect = Rectangle(transform, UI_BTN_BG);

    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
    E.entities[E.ROLLING_INDEX].type = EntityType::Button;

    auto parent_size = v2(transform.size);
    auto parent_pos = v2(transform.position);
    CenterChildInParent(&parent_pos, &parent_size, &pos, &text_size);
    auto text_info = AddText(text, pos, WHITE, font_size);
    auto rect_index = R.font_buffer.AddRect(rect);

    E.entities[E.ROLLING_INDEX].button = ButtonInfo(text_info, rect_index, parent_pos, parent_size, on_click);
    E.ROLLING_INDEX++;
}

void StackLayout::PositionChild(v2 own_pos, v2& pos, v2 child_size, u16 child_index)
{
    switch (dir) {
    case Direction::Vertical:
        pos.y = own_pos.y + total_height;
        if (child_index > 0)
            pos.y += spacing;
        pos.x = own_pos.x;
        total_height += child_size.y + spacing;
        break;
    case Direction::Horizontal:
        pos.x = own_pos.x + total_width;
        if (child_index > 0)
            pos.x += spacing;
        pos.y = own_pos.y;
        total_width += child_size.x + spacing;
        break;
    }
}

void Layout::PositionChild(v2& pos, v2 child_size)
{
    switch (type) {
    case LayoutType::Stack:
        stack.PositionChild(this->pos, pos, child_size, number_of_children);
        break;
    }
    number_of_children++;
}

void Gui::DrawIconAndLabel(IconParams icon_params, std::string label, v2 pos, u8 font_size)
{
    if (!layouts.empty()) {
        Layout& parent = layouts.back();

        parent.PositionChild(pos, icon_params.size);
    }

    auto icon_index = icon_params.index;
    auto icon_size = icon_params.size;

    auto atlas = RM.GetTexture("icons");
    auto info = GetTextureInfoByIndex(icon_index, icon_size, "icons");
    info.scale = v2(icon_params.scale);

    R.icons_buffer.AddTexturedRect(&info, atlas, pos, icon_size);

    pos.x += icon_size.x * icon_params.scale + icon_params.padding;
    pos.y += (icon_size.y * icon_params.scale) / 2;

    AddText(label, pos, WHITE, font_size);
}

void Gui::Stack(Direction dir, u8 spacing, v2 pos)
{
    Layout layout;
    layout.stack = StackLayout();
    layout.stack.spacing = spacing;
    layout.stack.dir = dir;
    layout.pos = pos;
    layout.type = LayoutType::Stack;

    assert(pos.x != -1 && layouts.empty());

    if (!layouts.empty()) {
        layout.pos = { -1, -1 };
    }

    layouts.push(layout);
}

void Gui::EndLayout()
{
    layouts.pop();
}
