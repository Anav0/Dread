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

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_truetype.h"

#include "Atlas.h"

#include <algorithm>
#include <set>
#include <windows.h>

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
    // STATE = GameState();
    STATE.window = WindowManager();
    STATE.window.camera = Camera(v3(0.22f, 22.0f, 10.0f), -84.0f, -67.0f);

    if (!STATE.window.Init()) {
        return -1;
    }

    WindowManager* window = &STATE.window;
    Camera* camera = &STATE.window.camera;

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

    /*
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    */

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    stbi_set_flip_vertically_on_load(true);

    u8 size = 36;
    // TR.BakeFont("oswald.ttf", "oswald", { size }, BakeMode::WriteIfNoneExist);
    // TR.UseFont("oswald.ttf");
    TR.BakeFont("noto.ttf", "noto", { size }, BakeMode::WriteIfNoneExist);
    TR.UseFont("noto.ttf");

	RM.LoadRequiredResources();
  
	R.object_shader = RM.GetShader("object");

    RM.LoadModel("map/map.obj", "map");
    RM.LoadModel("sphere/sphere.obj", "sphere");

    auto map_buffer_data = AddModel({ 0, 0, 0 }, "map", GREY, 0, 1);

    // TODO: move to some place better
    R.font_buffer.Allocate();
    R.icons_buffer.Allocate();
    R.icons_buffer.texture_key = "icons";

    std::vector<MeshInBuffer> meshes;

    int i = 0;

#if 1
    for (auto& mesh : map_buffer_data) {
        if (i > NUMBER_OF_OBLASTS - 1)
            continue;

        auto code = static_cast<OblastCode>(i);
        auto control = INITIAL_CONTROL.at(code);

        ID id = E.CreateOblast(Oblast(mesh, static_cast<OblastCode>(i), OBLAST_NAMES.at(code), control));
        SetupBoundingBox(mesh, id);

        i++;
    }
#endif

    m4 projection = glm::perspective(glm::radians(camera->zoom), (f32)STATE.window.screen_size.x / (f32)STATE.window.screen_size.y, 0.1f, 1000.0f);
    m4 ortho_projection = glm::ortho(0.0f, (f32)STATE.window.screen_size.x, 0.0f, (f32)STATE.window.screen_size.y);

    R.projection = projection;
    R.ortho_projection = ortho_projection;

    std::vector<Line> lines;

    auto ui_oblast_control = AddText(std::string("Control: 100.00%").size(), { 20, GetScreenSize().y - 50 }, WHITE, size);

    UI.DrawBtn("Increase control", size, { 200, 200 }, []() { ChangeControl(0.1); });
    UI.DrawBtn("Decrease control", size, { 200, 250 }, []() { ChangeControl(-0.1); });

    DrawResources(size);

    while (!STATE.window.IsClosing()) {
        STATE.window.onBeginOfTheLoop();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        E.Update();
        R.Update();

        if (STATE.window.buttonAction == MouseAction::PRESSED && STATE.window.buttonType == MouseButton::LEFT) {
            Ray ray = GetRayFromEyes(&projection);
            lines.push_back(Line(ray.position, ray.position + ray.direction * 1000.0f));
            Collision c = CheckRayCollision(ray, projection);
            if (c.hit_something) {
                switch (c.what_was_hit) {
                case EntityType::BoundingBox:
                    auto entity = E.GetEntityById(c.box.child_id);
                    if (entity->type == EntityType::Oblast) {
                        f32 region_control = entity->oblast.ukrainian_control;
                        ui_oblast_control.ChangeText(std::format("Control: {:.2f}%", region_control*100), size, { 20, GetScreenSize().y - 50 });
                    }
                    break;
                }
            } else {
                printf("Nothing!\n");
            }
        }

        R.Draw();

#if 0
        for (auto& line : lines)
            line.Draw(line_shader, &projection);
#endif

        printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

        glfwSwapBuffers(window->window);
    }

    glfwTerminate();
    return 0;
}
