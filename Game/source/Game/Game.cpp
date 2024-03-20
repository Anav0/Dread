#include "Engine/Base.h"
#include "Engine/Buffers.h"
#include "Engine/Particles.h"
#include "Engine/Gui.h"
#include "Engine/Renderer.h"
#include "Engine/Distribution.h"
#include "Engine/Animator.h"

#include "Engine/WindowManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "GameState.h"
#include "RenderHelpers.h"

#include "Entities.cpp"
#include "EntityManager.cpp"
#include "GameState.cpp"
#include "RenderHelpers.cpp"
#include "glad/glad.h"

#include <execution>
#include <algorithm>
#include <chrono>

using namespace std::chrono;

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

RandomDist placement_x = RandomDist(0, 0, 0);
RandomDist placement_y = RandomDist(0, 0, 0);
RandomDist direction = RandomDist(0, 0, 0);
RandomDist velocity  = RandomDist(0, 0, 0);
RandomDist ttl       = RandomDist(0, 0, 0);
RandomDist rgb       = RandomDist(0, 0, 0);

std::vector<Particle> particles;
BufferLayout emitter_layout {
	{ BufferElementType::VFloat4, "model_0"},
	{ BufferElementType::VFloat4, "model_1"},
	{ BufferElementType::VFloat4, "model_2"},
	{ BufferElementType::VFloat4, "model_3"},
	{ BufferElementType::VFloat4, "color"},
	{ BufferElementType::VFloat2, "pos"},
	{ BufferElementType::VFloat2, "size"},
	{ BufferElementType::VFloat2, "initial_size"},
	{ BufferElementType::VFloat2, "direction"},
	{ BufferElementType::Float, "velocity"},
	{ BufferElementType::Float, "ttl_s"},
};

void GameUpdateAndRender(WindowManager* window)
{
    info.action = window->buttonAction;
    info.type = window->buttonType;
    info.pos.x = window->mouse_x;
    info.pos.y = window->mouse_y;

    assert(glClearColor != NULL);

    E.Update();

    UI.onFrameBegin(info);

    R.Update(window->delta_time);

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

	//DrawBeamQuad({200, 200}, { 50, 50}, window->screen_size, window->delta_time, GOLD);

    //DrawUI(window);

		if(info.action == MouseAction::PRESSED && info.type == MouseButton::LEFT) {
			i32 entity_id = picking_buffer.ReadPixel(info.pos);
			if(entity_id != -1) {
				auto entity = E.GetEntityById(entity_id);
				if (entity->type == EntityType::Oblast) {
					  STATE.selected_oblast = entity->oblast.code;
						entity->oblast.mesh.ChangeColor(YELLOW);
				}
			}
		}
		//--------------------------------------------------------------

		auto p_shader = RM.GetShader("picking");
		picking_buffer.Bind();
    glClearColor(-1.0f, -1.0f, -1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    R.DrawModels(p_shader, &picking_buffer, window->camera, window->screen_size);
	picking_buffer.Unbind();

	//--------------------------------------------------------------
						
	auto single_color_shader = RM.GetShader("simple");
	auto shader = RM.GetShader("object");

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
	R.ScaleAllModels(1.0);
    R.DrawModels(shader, &picking_buffer, window->camera, window->screen_size);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	R.ScaleAllModels(1.02);
    R.DrawModels(single_color_shader, &picking_buffer, window->camera, window->screen_size);
		
    R.Flush();

    R.Draw(shader, window->camera, window->screen_size);

	auto& camera = window->camera;
    //printf("Camera: %f %f %f | %f %f\r", camera.position.x, camera.position.y, camera.position.z, camera.yaw, camera.pitch);

    R.Reset();
    UI.Reset();

    STATE.turn_changed = false;
}

void GlInit() {
#if CULLING
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#endif
    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
 
}

void GameInitAfterReload(WindowManager* window)
{
    gladLoadGL();

		picking_buffer.Allocate(window->screen_size);

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

		GlInit(); 
    stbi_set_flip_vertically_on_load(true);

		R.buffers.clear();
    R.Init(window->camera, window->screen_size);

    RM.LoadRequiredResources();
    u8 size = 38;
    TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("oswald.ttf");
}

void ColorChange(WindowManager* window, std::string& particle_id, Particle& p, f32 dt) {
	static f32 change_dir_every_ms_b = 100;
	static f32 change_dir_every_ms = change_dir_every_ms_b;
	change_dir_every_ms -= dt;

	if(change_dir_every_ms < 0) {
		p.direction = v2(direction.GenerateSingle() / 100, direction.GenerateSingle() / 100);
		change_dir_every_ms = change_dir_every_ms_b;
	}

	p.color.w = p.ttl_s * 1.1;
	p.size *= p.ttl_s / p.size;
}

void SetupEmitter(WindowManager* window) {
	  //TODO: tmp
		R.emitters.clear();
		ParticlesEmitter emitter = ParticlesEmitter(&placement_x, &placement_y, &direction, &velocity, &ttl, &rgb);
		emitter.Allocate(emitter_layout);
		v2 pos  = { window->screen_size.x-100, window->screen_size.y-85 };
		//v2 pos  = {200, 200};
		v2 size = v2(85);
		u64 n = 75;
	
		placement_x.SetParams(n, pos.x, pos.x + size.x);
		placement_y.SetParams(n, pos.y, pos.y + size.y);
		velocity.SetParams(n, 10, 20);
		direction.SetParams(n, -100, 100);
		ttl.SetParams(n, 1000, 3000);
		rgb.SetParams(3, 0, 1000);

		std::vector<Keyframes<v4>> frames;
		frames.reserve(n);

		for(u64 i = 0; i < n; i++) {
			auto a = rgb.Generate();
			auto b = rgb.Generate();
			auto c = rgb.Generate();

		Keyframes<v4> keyframes;
		KeyFrame<v4> frame;

		//frame.duration = duration_cast<milliseconds>(duration<u32>(static_cast<u32>(velocity.GenerateSingle() / 100)));
		frame.duration = 1000ms;
		frame.target_value = YELLOW;
		keyframes.push_back(frame);

		//frame.duration = duration_cast<milliseconds>(duration<u32>(static_cast<u32>(velocity.GenerateSingle() / 100)));
		frame.duration = 1000ms;
		frame.target_value = RED;
		keyframes.push_back(frame);

		frames.push_back(keyframes);
		}

		emitter.Init(window, n, pos, size, v2(200));
		emitter.SetKeyframes(frames);
		emitter.Allocate(emitter_layout);
		emitter.update = ColorChange;

		R.emitters.push_back(emitter);
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
		GlInit(); 
    stbi_set_flip_vertically_on_load(true);

		SetupEmitter(window);
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

		GlInit(); 
    stbi_set_flip_vertically_on_load(true);

		SetupEmitter(window);

    RM.LoadRequiredResources();
    u8 size = 38;
    TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("oswald.ttf");

    R.Init(window->camera, window->screen_size);

    AddMap();

    return &STATE;
}
