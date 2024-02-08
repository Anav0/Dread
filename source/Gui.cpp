#include "Gui.h"
#include "EntityManager.h"
#include "Misc.h"
#include "RenderHelpers.h"
#include "Renderer.h"
#include "TextRenderer.h"

Gui UI;

void StackLayout::PositionChild(v2 own_pos, v2& pos, v2 child_size, u16 child_index)
{
    switch (dir) {
    case Direction::Vertical:
        pos.y = own_pos.y + total_height;
        if (child_index > 0) {
            pos.y += spacing;
            total_height += child_size.y + spacing;
        } else {
            total_height += child_size.y;
        }
        pos.x = own_pos.x;
        break;
    case Direction::Horizontal:
        pos.x = own_pos.x + total_width;
        if (child_index > 0) {
            pos.x += spacing;
            total_width += child_size.x + spacing;
        } else {
            total_width += child_size.x;
        }
        pos.y = own_pos.y;
        break;
    }

    //printf("Child %i, X: %f, Y: %f, W: %f, H: %f\n", child_index, pos.x, pos.y, child_size.x, child_size.y);
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

//------------------------------------------------------------------------

void Gui::Stack(Direction dir, u8 spacing, v2 pos)
{
    Layout layout;
    layout.stack = StackLayout();
    layout.stack.spacing = spacing;
    layout.stack.dir = dir;
    layout.pos = pos;
    layout.type = LayoutType::Stack;

    if (!layouts.empty()) {
        auto& parent = layouts.back();
        parent.PositionChild(layout.pos, v2(0));
    }

    layouts.push_back(layout);
}

void Gui::EndLayout()
{
    layouts.pop_back();
}

//------------------------------------------------------------------------

void Gui::DrawLabel(std::string label, u8 max_chars, v2 pos, TextStyle style, bool use_layout)
{
    FontInfo font = TR.GetCurrentFont(style.font_size);
    Texture* font_atlas = RM.GetTexture(font.path);

    if (max_chars < label.size()) {
        max_chars = label.size();
    }

    if (font_atlas == nullptr)
        font_atlas = RM.LoadTexture(font.path, font.path, true, true);

    R.font_buffer.texture_key = font.path;

    if (use_layout && !layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, TR.GetTextSize(label.c_str(), style.font_size));
    }

    f32 base_y = pos.y;

    v4 color_to_use;
    for (size_t i = 0; i < max_chars; i++) {
        char c = ' ';
        color_to_use = style.color;

        if (i < label.size()) {
            c = label[i];
        }

        if (c == ' ') {
            color_to_use = TRANSPARENT;
        }

        GlyphInfo glyph = font.glyphs[c];

        AtlasTextureInfo texture_info;
        texture_info.position = v2(glyph.x, glyph.y);
        texture_info.scale = v2(1.0f, 1.0f);
        texture_info.size = v2(glyph.w, glyph.h);
        R.font_buffer.AddTexturedRect(&texture_info, font_atlas, pos, texture_info.size, 0, color_to_use);

        // pos.x += glyph.advance;
        pos.x += glyph.w;
        pos.y = base_y;
    }
}

bool Gui::DrawButton(const char* label, v2 pos, ButtonStyle style)
{
    auto mouse_x = STATE.window.mouse_x;
    auto mouse_y = STATE.window.mouse_y;

    v2 text_size = TR.GetTextSize(label, style.font_size);

    u8 padding_x = style.padding.x;
    u8 padding_y = style.padding.y;

    Transform transform {};
    transform.size = v3(text_size.x + padding_x, text_size.y + padding_y, 0.0);
    transform.scale = v3(1.0);
    transform.rotation = 0.0;

    if (!layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, transform.size);
    }
    transform.position = v3(pos.x, pos.y, 0.0);

    v4 bg_color = style.bg_color;
    bool is_mouse_over = isPointInRect(transform.position, transform.size, mouse_x, mouse_y);
    if (is_mouse_over) {
        bg_color = YELLOW;
    }

    Rectangle rect = Rectangle(transform, bg_color);

    auto parent_size = v2(transform.size);
    auto parent_pos = v2(transform.position);

    CenterChildInParent(&parent_pos, &parent_size, &pos, &text_size);
    TextStyle text_style = default_style;
    text_style.font_size = style.font_size;
    R.ui_buffer.AddRect(rect);

    Gui::DrawLabel(label, 0, pos, text_style, false);

    return is_mouse_over && STATE.window.buttonType == MouseButton::LEFT && STATE.window.buttonAction == MouseAction::RELEASED;
}

void Gui::DrawIcon(IconParams icon_params, v2 pos)
{
    if (!layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, icon_params.size);
    }

    auto icon_size = icon_params.size;

    auto atlas = RM.GetTexture("icons");
    auto info = GetTextureInfoByIndex(icon_params.index, icon_size, "icons");
    info.scale = v2(icon_params.scale);

    R.ui_buffer.AddTexturedRect(&info, atlas, pos, icon_size);
}

void Gui::DrawIconAndLabel(IconParams icon_params, std::string label, v2 pos, TextStyle style)
{
    if (!layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, icon_params.size + TR.GetTextSize(label.c_str(), style.font_size));
    }

    auto icon_size = icon_params.size;

    auto atlas = RM.GetTexture("icons");
    auto info = GetTextureInfoByIndex(icon_params.index, icon_size, "icons");
    info.scale = v2(icon_params.scale);

    R.ui_buffer.AddTexturedRect(&info, atlas, pos, icon_size);

    pos.x += icon_size.x * icon_params.scale + icon_params.padding;
    pos.y += (icon_size.y * icon_params.scale) / 2;

    Gui::DrawLabel(label, 0, pos, style, false);
}

//------------------------------------------------------------------------

void Gui::Reset()
{
    layouts.clear();
}