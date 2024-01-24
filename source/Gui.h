#ifndef GUI_H
#define GUI_H

#pragma once

#include "Constants.h"

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

class Gui {
    const std::string ui_atlas = "ui_atlas";

public:

	void DrawLabel(std::string text, v2 pos, v4 color, TextStyle style = default_style);
	bool DrawBtn(const char* text, u8 font_size, v2 pos);
    void BeginLayout(Layout layout, u8 gap = 8);
    void EndLayout();
};

extern Gui UI;

#endif
