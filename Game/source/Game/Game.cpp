#include "Engine/Renderer.h"
#include "Engine/Gui.h"

#include "Game.h"
#include "Atlas.h"
#include "Entities.h"
#include "GameState.h"
#include "EntityManager.h"
#include "RenderHelpers.h"
#include <glad/glad.h>

#include "Entities.cpp"
#include "GameState.cpp"
#include "EntityManager.cpp"
#include "RenderHelpers.cpp"

#include "glad.c"

void GameUpdateAndRender(WindowManager* window)
{
    //E.Update();
    //R.Update();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;

    /*
    if (STATE.window.buttonAction == MouseAction::PRESSED && STATE.window.buttonType == MouseButton::LEFT) {
        Ray ray = GetRayFromEyes(&R.projection);
#if DEBUG_LINES
        lines.push_back(Line(ray.position, ray.position + ray.direction * 1000.0f));
#endif
        Collision c = CheckRayCollision(ray, R.projection);
        if (c.hit_something) {
            switch (c.what_was_hit) {
            case EntityType::BoundingBox:
                auto entity = E.GetEntityById(c.box.child_id);
                if (entity->type == EntityType::Oblast) {
                    STATE.selected_oblast = entity->oblast.code;
                }
                break;
            }
        } else {
            printf("Nothing hit with mouse ray!\n");
        }
    }
    */

    const Gradient card_gradient {
        GradientType::Radial,
        v2(0.5, 0.5),
        { 0.777, -0.56 },
        1.0,
        { YELLOW, BLACK },
    };

    const Gradient header_gradient {
        GradientType::ThreeColor,
        v2(0.25, 0.65),
        { 0.0, 0.0 },
        1.0,
        { RED, BLACK, YELLOW },
    };

    R.gradient_buffer.AddGradient({ 0, window->screen_size.y - HEADER_H }, { window->screen_size.x, HEADER_H }, header_gradient); 
    R.gradient_buffer.AddGradient({ 50, 50 }, { 260, 130 }, card_gradient);

    DrawUI(window);

    R.Flush();

    R.Draw(window->camera, window->screen_size);

#if DEBUG_LINES
    for (auto& line : lines)
        line.Draw(line_shader, &projection);
#endif

    // printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

    R.Reset();
    UI.Reset();

    STATE.turn_changed = false;
}

void GameInit(WindowManager* window)
{
    RM.LoadRequiredResources();
    u8 size = 38;
    TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("oswald.ttf");

    R.Init(window->camera, window->screen_size);

    AddMap();
    AddSupportingCountries();

    SupportPackage p1 {
        "Polish support",
        3,
        CountryCode::PL
    };

    p1.delivery.push_back({ 10, GetBmp1() });
    p1.delivery.push_back({ 20, GetT72() });

    PromiseSupport(p1);

    SupportPackage p2 {
        "USA support",
        4,
        CountryCode::USA
    };

    p2.delivery.push_back({ 5, GetBmp1() });
    p2.delivery.push_back({ 5, GetT72() });

    PromiseSupport(p2);
}
