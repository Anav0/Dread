#include "Gui.h"
#include "EntityManager.h"
#include "Misc.h"
#include "RenderHelpers.h"
#include "Renderer.h"
#include "TextRenderer.h"

Gui UI;

LabelHandle Gui::DrawLabel(std::string label, v2 pos, TextStyle style, bool use_layout) {
	FontInfo font       = TR.GetCurrentFont(style.font_size);
    Texture* font_atlas = RM.GetTexture(font.path);

    if (font_atlas == nullptr)
        font_atlas = RM.LoadTexture(font.path, font.path, true, true);

    R.font_buffer.texture_key = font.path;

	if (use_layout && !layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, TR.GetTextSize(label.c_str(), style.font_size));
    }

    f32 base_y = pos.y;
    TextInBuffer text {};
    text.pos_in_buffer = R.font_buffer.GetCurrentIndex();
	text.font_size     = style.font_size;
	text.last_pos      = pos;

    for (size_t i = 0; i < label.size(); i++) {
        char c = label[i];

        GlyphInfo glyph = font.glyphs[c];

        if (c != ' ') {
            AtlasTextureInfo texture_info;
            texture_info.position = v2(glyph.x, glyph.y);
            texture_info.scale = v2(1.0f, 1.0f);
            texture_info.size = v2(glyph.w, glyph.h);
            R.font_buffer.AddTexturedRect(&texture_info, font_atlas, pos, texture_info.size, 0, style.color);
        }
		
        // pos.x += glyph.advance;
        pos.x += glyph.w;
        pos.y = base_y;
        text.length += 1;
    }

    UIElement el {};
    el.type = UIElementType::Label;
    el.label = text;
	this->elements.push_back(el);

	LabelHandle handle2 {};
	handle2.index = this->elements.size()-1;
	return handle2;
}

ButtonHandle Gui::DrawBtn(const char* label, u8 font_size, void on_click(), v2 pos, bool use_layout)
{
    auto mouse_x = STATE.window.mouse_x;
    auto mouse_y = STATE.window.mouse_y;

    v2 text_size = TR.GetTextSize(label, font_size);

    u8 padding_x = 30;
    u8 padding_y = 20;

    Transform transform {};
    transform.size     = v3(text_size.x + padding_x, text_size.y + padding_y, 0.0);
    transform.scale    = v3(1.0);
    transform.rotation = 0.0;

	if (use_layout && !layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, transform.size);
    }
    transform.position = v3(pos.x, pos.y, 0.0);

    Rectangle rect = Rectangle(transform, UI_BTN_BG);

    auto parent_size = v2(transform.size);
    auto parent_pos = v2(transform.position);
    CenterChildInParent(&parent_pos, &parent_size, &pos, &text_size);
	TextStyle style = default_style;
	style.font_size = font_size;
    auto rect_index = R.ui_buffer.AddRect(rect);

    UIElement el {};
    el.type = UIElementType::Button;
	LabelHandle label_handle = Gui::DrawLabel(label, pos, style, false);
    el.button = ButtonInBuffer(label_handle, rect_index, parent_pos, parent_size, on_click);
	this->elements.push_back(el);

	ButtonHandle handle {};
	handle.label_handle = label_handle;
	handle.index = this->elements.size()-1;
	return handle;
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

LabelHandle Gui::DrawIconAndLabel(IconParams icon_params, std::string label, v2 pos, u8 font_size)
{
    if (!layouts.empty()) {
        Layout& parent = layouts.back();
        parent.PositionChild(pos, icon_params.size + TR.GetTextSize(label.c_str(), font_size));
    }

    auto icon_size  = icon_params.size;

    auto atlas = RM.GetTexture("icons");
    auto info = GetTextureInfoByIndex(icon_params.index, icon_size, "icons");
    info.scale = v2(icon_params.scale);

    R.ui_buffer.AddTexturedRect(&info, atlas, pos, icon_size);

    pos.x += icon_size.x * icon_params.scale + icon_params.padding;
    pos.y += (icon_size.y * icon_params.scale) / 2;

	TextStyle style = default_style;
	style.font_size = font_size;
    
	return Gui::DrawLabel(label, pos, style, false);
}

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

void Gui::Update()
{
	for (auto & e : elements) {
		switch (e.type) {
			case UIElementType::Button:
				e.button.Update();
				break;
		}
	}
}

void LabelHandle::UpdateColor(v4 color) {
	auto el = &UI.elements[index];
	el->label.ChangeColor(color);
}

void ButtonHandle::UpdateBg(v4 color) { 
	auto el = &UI.elements[index];
	el->button.UpdateBg(color);
}

void LabelHandle::UpdateText(std::string label){
	auto el = &UI.elements[index];
	el->label.ChangeText(label);
}

void ButtonHandle::UpdateText(std::string label){
	label_handle.UpdateText(label);
}