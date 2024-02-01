#ifndef GUI_H
#define GUI_H

#pragma once

#include "Constants.h"
#include "GameState.h"
#include "Renderer.h"
#include "TextRenderer.h"

#include <queue>
#include <string>

class Texture;

enum class Direction {
    Vertical,
    Horizontal,
};

struct TextStyle {
    u16 ch_max_width = 0;
    u16 ch_max_height = 0;
    v4 color = WHITE;
};

constexpr TextStyle default_style {
    96,
    0,
    WHITE,
};

struct ButtonInfo {
    TextInBuffer text_info;
    u32 bg_index_in_ui_buffer;
    v2 pos, size;
    void (*on_click)(void);
    bool was_hovered = false;

    ButtonInfo(TextInBuffer text_info, u32 bg_index_in_ui_buffer, v2 pos, v2 size, void (*on_click)(void))
    {
        this->text_info = text_info;
        this->bg_index_in_ui_buffer = bg_index_in_ui_buffer;
        this->pos = pos;
        this->size = size;
        this->on_click = on_click;
    }

    void Update()
    {
        auto mouse_x = STATE.window.mouse_x;
        auto mouse_y = STATE.window.mouse_y;
        auto mouse_over = isPointInRect(pos, size, mouse_x, mouse_y);

        if (mouse_over) {
            if (!was_hovered)
                R.font_buffer.UpdateColor(bg_index_in_ui_buffer, UI_BTN_HOVER_BG);

            was_hovered = true;
            if (STATE.window.buttonAction == MouseAction::PRESSED && STATE.window.buttonType == MouseButton::LEFT) {
                on_click();
            }
        } else {
            if (was_hovered) {
                R.font_buffer.UpdateColor(bg_index_in_ui_buffer, UI_BTN_BG);
            }
            was_hovered = false;
        }
    }
};

// TODO: move
constexpr u16 SWORD = 12;
constexpr u16 MEGAPHONE = 5;

struct IconParams {
    u16 index;
    v2 size;
    f32 scale;
    f32 padding;
};

enum class LayoutType {
    Stack
};

struct StackLayout {
    u8 spacing;
    Direction dir;
    u16 total_height, total_width;
    void PositionChild(v2, v2&, v2, u16);
};

struct Layout {
    i16 parent_id = -1; //-1 means global canvas that - just like god - exists only in my head
    v2 pos = { -1, -1 };
    LayoutType type;
    u16 number_of_children = 0;
    union {
        StackLayout stack;
    };

    void PositionChild(v2& pos, v2 child_size);
};

class Gui {
    const std::string ui_atlas = "ui_atlas";
    std::deque<Layout> layouts;

public:
    void DrawIconAndLabel(IconParams params, std::string label, v2 pos, u8 font_size);
    void DrawLabel(std::string text, v2 pos, v4 color, TextStyle style = default_style);
    void DrawBtn(const char* text, u8 font_size, v2 pos, void on_click());
    void Stack(Direction layout, u8 spacing = 20, v2 pos = { 0, 0 });
    void EndLayout();
};

extern Gui UI;

#endif
