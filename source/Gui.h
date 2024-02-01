#ifndef GUI_H
#define GUI_H

#pragma once

#include "Constants.h"
#include "GameState.h"
#include "Renderer.h"
#include "TextRenderer.h"

#include <queue>
#include <string>
#include <vector>

class Texture;

enum class Direction {
    Vertical,
    Horizontal,
};

struct TextStyle {
    u16 ch_max_width = 0;
    u16 ch_max_height = 0;
    v4 color = WHITE;
    u8 font_size = 36;
};

constexpr TextStyle default_style {
    96,
    0,
    WHITE,
    36
};

struct LabelHandle {
    friend class Gui;
private:
    u32 index;
public:
    void UpdateColor(v4 color);
    void UpdateText(std::string label);
};

struct ButtonHandle {
    friend class Gui;
private:
    u32 index;
	LabelHandle label_handle;

public:
    void UpdateBg(v4 color);
    void UpdateText(std::string label);
};

struct ButtonInBuffer {
    LabelHandle text_handle;
    u32 bg_index_in_ui_buffer;
    v2 pos, size;
    void (*on_click)(void);
    bool was_hovered = false;
	v4 color = UI_BTN_BG;

    ButtonInBuffer() { }

    ButtonInBuffer(LabelHandle text_handle, u32 bg_index_in_ui_buffer, v2 pos, v2 size, void (*on_click)(void))
    {
        this->text_handle = text_handle;
        this->bg_index_in_ui_buffer = bg_index_in_ui_buffer;
        this->pos = pos;
        this->size = size;
        this->on_click = on_click;
    }

    void UpdateBg(v4 color)
    {
		this->color = color;
        R.ui_buffer.UpdateColor(bg_index_in_ui_buffer, color);
    }

    void Update()
    {
        auto mouse_x = STATE.window.mouse_x;
        auto mouse_y = STATE.window.mouse_y;
        auto mouse_over = isPointInRect(pos, size, mouse_x, mouse_y);

        if (mouse_over) {
            if (!was_hovered)
                R.ui_buffer.UpdateColor(bg_index_in_ui_buffer, UI_BTN_HOVER_BG);

            was_hovered = true;
            if (STATE.window.buttonAction == MouseAction::PRESSED && STATE.window.buttonType == MouseButton::LEFT) {
                on_click();
            }
        } else {
            if (was_hovered) {
                R.ui_buffer.UpdateColor(bg_index_in_ui_buffer, this->color);
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

enum class UIElementType {
    Button,
    Label
};

class UIElement {
public:
    UIElementType type;
    union {
        ButtonInBuffer button;
        TextInBuffer label;
    };
};

class Gui {
    friend class LabelHandle;
    friend class ButtonHandle;

    std::deque<Layout> layouts;
    std::vector<UIElement> elements;

public:
    LabelHandle DrawIconAndLabel(IconParams params, std::string label, v2 pos, u8 font_size);
    LabelHandle DrawLabel(std::string label, v2 pos = { 0, 0 }, TextStyle style = default_style, bool use_layout = true);
    ButtonHandle DrawBtn(const char* label, u8 font_size, void on_click(), v2 pos = { 0, 0 }, bool use_layout = true);
    void Stack(Direction layout, u8 spacing = 20, v2 pos = { 0, 0 });
    void EndLayout();
    void Update();
};

extern Gui UI;

#endif
