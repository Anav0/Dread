#include "Engine/Buffers.h"
#include "Engine/Gui.h"
#include "Engine/Renderer.h"

#include "Atlas.h"
#include "Engine/WindowManager.h"
#include "Entities.h"
#include "EntityManager.h"
#include "Game.h"
#include "GameState.h"
#include "RenderHelpers.h"

#include "Entities.cpp"
#include "EntityManager.cpp"
#include "GameState.cpp"
#include "RenderHelpers.cpp"
#include "glad/glad.h"

u64 frame_counter = 0;
MouseInfo info;

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

PickingBuffer picking_buffer;

void GameUpdateAndRender(WindowManager* window)
{
    info.action = window->buttonAction;
    info.type = window->buttonType;
    info.pos.x = window->mouse_x;
    info.pos.y = window->mouse_y;

    E.Update();
    assert(glClearColor != NULL);

    UI.onFrameBegin(info);

    R.Update();

    if (frame_counter++ > 144) {
        RM.HotReloadShaders();
        frame_counter = 0;
    }

    const Gradient card_gradient {
        GradientType::Radial,
        v2(0.25, 0.5),
        { 0.777, -0.56 },
        1.0,
        { YELLOW, BLACK },
    };

    const Gradient header_gradient {
        GradientType::ThreeColor,
        v2(0.5, 0.65),
        { 0.0, 0.0 },
        1.0,
        { RED, BLACK, YELLOW },
    };

    R.gradient_buffer.AddGradient({ 0, window->screen_size.y - HEADER_H }, { window->screen_size.x, HEADER_H }, header_gradient);
    R.gradient_buffer.AddGradient({ 50, 50 }, { 260, 130 }, card_gradient);

    DrawUI(window);

    R.Flush();

		//--------------------------------------------------------------
		auto p_shader = RM.GetShader("picking");
		picking_buffer.Bind();
    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    R.Draw(p_shader, &picking_buffer, window->camera, window->screen_size);
		picking_buffer.Unbind();
		//--------------------------------------------------------------
		//

		//TODO: it is not really entity id but mesh id
		if(info.action == MouseAction::PRESSED && info.type == MouseButton::LEFT) {
			u32 entity_id = picking_buffer.ReadPixel(info.pos);
			auto entity = E.GetEntityById(entity_id);
      if (entity->type == EntityType::Oblast) {
          STATE.selected_oblast = entity->oblast.code;
      }
		}
				
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

		auto shader = RM.GetShader("object");
    R.Draw(shader, &picking_buffer, window->camera, window->screen_size);

#if DEBUG_LINES
    for (auto& line : lines)
        line.Draw(line_shader, &projection);
#endif

    // printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

    R.Reset();
    UI.Reset();

    STATE.turn_changed = false;
}

void GameInitAfterReload(WindowManager* window)
{
    gladLoadGL();

		picking_buffer.Allocate(window->screen_size);

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

#if CULLING
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#endif

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    stbi_set_flip_vertically_on_load(true);

    R.Init(window->camera, window->screen_size);

    RM.LoadRequiredResources();
    u8 size = 38;
    TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("oswald.ttf");
}

GameState* GameInit(WindowManager* window)
{
    gladLoadGL();

		picking_buffer.Allocate(window->screen_size);

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

#if CULLING
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#endif

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    stbi_set_flip_vertically_on_load(true);

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

    return &STATE;
}

GameState* GameInitEx(GameState state, WindowManager* window)
{
    STATE = state;
    gladLoadGL();

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

#if CULLING
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#endif

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    stbi_set_flip_vertically_on_load(true);

    RM.LoadRequiredResources();
    u8 size = 38;
    TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("oswald.ttf");

    R.Init(window->camera, window->screen_size);

    AddMap();

    return &STATE;
}
