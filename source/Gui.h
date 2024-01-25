#ifndef GUI_H
#define GUI_H

#pragma once

#include "Constants.h"
#include "TextRenderer.h"
#include "GameState.h"

#include <string>

class Texture;

enum class Layout {
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
        if (STATE.window.buttonAction == MouseAction::PRESSED && STATE.window.buttonType == MouseButton::LEFT) {
            auto mouse_x = STATE.window.mouse_x;
            auto mouse_y = STATE.window.mouse_y;
            auto mouse_over = isPointInRect(pos, size, mouse_x, mouse_y);

            if (mouse_over) {
                on_click();
            }
        }
    }
};

class Gui {
    const std::string ui_atlas = "ui_atlas";

public:
    void DrawLabel(std::string text, v2 pos, v4 color, TextStyle style = default_style);
    void DrawBtn(const char* text, u8 font_size, v2 pos, void on_click());
    void BeginLayout(Layout layout, u8 gap = 8);
    void EndLayout();
};

extern Gui UI;

#endif
