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

void Gui::DrawIconAndLabel(u16 icon, std::string label, v2 pos, u8 font_size)
{
    auto icon_size = v2(64);
    auto atlas = RM.GetTexture("icons");
    auto info = GetTextureInfoByIndex(icon, icon_size, "icons");

    R.icons_buffer.AddTexturedRect(&info, atlas, pos, icon_size);

    pos.x += icon_size.x;
    AddText(label, pos, WHITE, font_size);
}
