#include "Gui.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "TextRenderer.h"

Gui UI;

bool Gui::DrawBtn(const char* text, u8 font_size, v2 pos)
{
    v2 text_size = TR.GetTextSize(text, font_size);

    Transform transform {};
    // transform.size = v3(text_size, 0.0);
    transform.size = v3(200, 200, 0.0);
    transform.position = v3(pos.x, pos.y, 0.0);
    transform.rotation = 0.0;
    transform.scale = v3(1.0);

    Rectangle rect = Rectangle(transform, UI_BTN_BG);

    E.entities[E.ROLLING_INDEX].id = E.ROLLING_INDEX;
    E.entities[E.ROLLING_INDEX].type = EntityType::Rect;
    E.entities[E.ROLLING_INDEX].rect = rect;
    E.ROLLING_INDEX++;

    R.ui_buffer.AddRect(rect);

    return false;
}
