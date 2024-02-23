#include "Camera.h"
#include "Collision.h"
#include "Constants.h"
#include "Entities.h"
#include "EntityManager.h"
#include "GameState.h"
#include "Gui.h"
#include "Line.h"
#include "Mesh.h"
#include "Model.h"
#include "Mouse.h"
#include "RenderHelpers.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "WindowManager.h"
#include "Weapons.h"
#include "HotReload.h"
#include "Atlas.h"
#include "Buffers.h"
#include "alloc.h"

#include "EntityManager.cpp"
#include "GameState.cpp"
#include "Animator.cpp"
#include "Gui.cpp"
#include "Renderer.cpp"
#include "RenderHelpers.cpp"
#include "ResourceManager.cpp"
#include "TextRenderer.cpp"
#include "Texture.cpp"
#include "WindowManager.cpp"
#include "BoundingBox.cpp"
#include "Buffers.cpp"
#include "Collision.cpp"
#include "Entities.cpp"
#include "HotReload.cpp"
#include "Line.cpp"
#include "Mesh.cpp"
#include "Model.cpp"
#include "Mouse.cpp"
#include "alloc.cpp"
#include "glad.c"

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_truetype.h"

#include <algorithm>
#include <set>

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

int main(int argc, char* argv[])
{
    if (!STATE.window.Init()) {
        printf("ERROR: Failed to initialize WindowManager\n");
        return -1;
    }

    auto window = &STATE.window;
    auto camera = &STATE.window.camera;

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

#if CULLING
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#endif

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    stbi_set_flip_vertically_on_load(true);

    GameCode game = LoadGameCode();

    RM.LoadRequiredResources();

    u8 size = 38;
    TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("oswald.ttf");

    R.Init();

#if DEBUG_LINES
    std::vector<Line> lines;
#endif
	AddMap();
	AddSupportingCountries();

	SupportPackage p1 {
		"Polish support",
		3,
		CountryCode::PL
	};

	p1.delivery.push_back( { 10, GetBmp1() });
	p1.delivery.push_back( { 20, GetT72() });

	PromiseSupport(p1);

	SupportPackage p2 {
		"USA support",
		4,
		CountryCode::USA
	};

	p2.delivery.push_back( { 5, GetBmp1() });
	p2.delivery.push_back( { 5, GetT72() });

	PromiseSupport(p2);

	Gradient header_gradient{};
	header_gradient.gradient_type = GradientType::ThreeColor;
	header_gradient.middle = v2(0.3, 0.65);
	header_gradient.colors[0] = RED;
	header_gradient.colors[1] = BLACK;
	header_gradient.colors[2] = YELLOW;

	/*
	Gradient card_gradient{};
	card_gradient.gradient_type = GradientType::Radial;
	card_gradient.middle = v2(0.5, 0.5);
	card_gradient.colors[0] = YELLOW;
	card_gradient.colors[1] = BLACK;
	card_gradient.radial_position = v2(0.777, -0.56);
	card_gradient.radial_factor = 1.00;
	*/
	
	u64 frame_counter=0;
	while (!STATE.window.IsClosing()) {
		STATE.window.onBeginOfTheLoop();
		glfwPollEvents();

		if(frame_counter++ > 165 * 4) {
			printf("Reload!\n\n\n\n");
			frame_counter = 0;
			UnloadGameCode(game);
			game = LoadGameCode();
		}
		
		//------------------------------------------------------------------------

		RM.HotReloadShaders();

		E.Update();
		R.Update();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		
		//------------------------------------------------------------------------

		R.gradient_buffer.AddGradient( { 0, STATE.window.screen_size.y - HEADER_H }, {STATE.window.screen_size.x, HEADER_H}, header_gradient);

		R.gradient_buffer.AddGradient( { 50, 50 }, {260, 130}, *game.card_gradient);

		DrawUI();

		R.Flush();

		R.Draw();

#if DEBUG_LINES
        for (auto& line : lines)
            line.Draw(line_shader, &projection);
#endif

    //printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

		//printf("UI buffer index: %i\n", R.ui_buffer.rolling_index);
		//printf("Font buffer index: %i\n", R.font_buffer.rolling_index);

				R.Reset();
				UI.Reset();

        STATE.turn_changed = false;

        glfwSwapBuffers(window->window);
    }


    glfwTerminate();
    return 0;
}
