#include "Camera.h"
#include "Collision.h"
#include "Constants.h"
#include "Entities.h"
#include "EntityManager.h"
#include "GameState.h"
#include "Mesh.h"
#include "Model.h"
#include "Mouse.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "WindowManager.h"

#include <set>

#define STB_IMAGE_IMPLEMENTATION
#include "RenderHelpers.h"
#include "stb_image.h"

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    stbi_set_flip_vertically_on_load(true);

    Shader* texture_shader = RM.LoadShader("texture.vert", "texture.frag", "texture");
    Shader* objects_shader = RM.LoadShader("object.vert", "object.frag", "object");
    Shader* debug_shader = RM.LoadShader("debug.vert", "debug.frag", "debug");
    Shader* mesh_shader = RM.LoadShader("mesh.vert", "mesh.frag", "mesh");

    R.object_shader = objects_shader;

    RM.LoadModel("map/map4.obj", "map");
    RM.LoadModel("sphere/sphere.obj", "sphere");
    RM.LoadModel("cube/cube.obj", "cube");

    auto map_buffer_data = AddModel({ 0, 0, 0 }, { 10, 10, 10 }, "map", GREY, 0.0f, 1.0f);

    std::vector<Oblast> oblasts;
    int i = 0;
    std::set<OblastCode> to_show = {
        Lviv,
        Donetsk,
        Crimea
    };
    STATE.bounding_draw_mode = BoundingDrawMode::ALL;

    std::vector<v4> pallette;
    pallette.push_back(RED);
    pallette.push_back(GOLD);
    pallette.push_back(YELLOW);

    for (auto& mesh : map_buffer_data) {
        //oblasts.push_back(Oblast(mesh, static_cast<OblastCode>(i), "Oblast", 1.0));

        auto color = pallette[i % pallette.size()];
        mesh.ChangeColor(color);
        i++;
    }

    m4 projection = glm::perspective(glm::radians(camera->zoom), (float)STATE.window.screen_size.x / (float)STATE.window.screen_size.y, 0.1f, 100.0f);

    R.projection = projection;

    while (!STATE.window.IsClosing()) {
        STATE.window.onBeginOfTheLoop();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (STATE.window.buttonAction == PRESSED && STATE.window.buttonType == LEFT) {
            Ray ray = GetRayFromEyes(&projection);
            Collision c = CheckRayCollision(ray, projection);
            if (c.hit_something) {
                switch (c.what_was_hit) {
                case eBoundingBox:
                    printf("Hit bounding box\n");
                    break;
                case eModel:
                    printf("Hit model, mesh index: %i\n", c.mesh_index);
                    break;
                }
            } else {
                printf("Nothing!\n");
            }
        }

        E.Update();

        R.Update();
        R.Draw();

        // printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

        glfwSwapBuffers(window->window);
    }

    glfwTerminate();
    return 0;
}
