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
    v2 pos = { -1, -1 };
    LayoutType type;
    u16 number_of_children = 0;
    union {
        StackLayout stack;
    };

    void PositionChild(v2& pos, v2 child_size);
};

struct ButtonStyle {
	v4 bg_color;
	v4 fg_color;
	v2 padding;
	u8 font_size;
};

constexpr ButtonStyle DEFAULT_BTN_SYTLE {
	RED,
	WHITE,
	v2(30,20),
	36,
};


class Gui {
    std::deque<Layout> layouts;

public:
    void Stack(Direction layout, u8 spacing = 20, v2 pos = { 0, 0 });
    void EndLayout();
	void Reset();

	bool DrawButton(const char* label, v2 pos = { 0, 0 }, ButtonStyle style = DEFAULT_BTN_SYTLE);
	void DrawLabel(std::string label, u8 max_chars = 0, v2 pos = { 0, 0 }, TextStyle style = default_style, bool use_layout = true);
	bool DrawIcon(IconParams params, v2 pos = { 0, 0 });
	bool DrawIcon(AtlasTextureInfo info, v2 pos = { 0, 0 });
	void DrawIconAndLabel(IconParams icon_params, std::string label, v2 pos = { 0, 0 }, TextStyle style = default_style);
};

extern Gui UI;

#endif
