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

    Shader* texture_shader = RM.LoadShader("texture.vert", "texture.frag", "texture");
    Shader* objects_shader = RM.LoadShader("object.vert", "object.frag", "object");
    Shader* debug_shader = RM.LoadShader("debug.vert", "debug.frag", "debug");

    R.object_shader = objects_shader;

    stbi_set_flip_vertically_on_load(true);

    // Model* sphere = RM.LoadModel("sphere/sphere.obj", "sphere");
    // R.models.push_back(sphere);

    RenderSphere({ 0, 0, 0 }, { 150, 150, 150 });
    RenderSphere({ 10, 0, 0 }, { 100, 100, 100 });

    // Model* ukraine_map = RM.LoadModel("map/map4.obj", "map");
    //  TODO: move to nice fn
    // R.models.push_back(ukraine_map);
    // u32 ukraine_map_model_index = R.models.size() - 1;
    //  ---
    std::vector<Oblast> oblasts;
    int i = 0;
    v4 color = { 0, 0, 0, 0 };
    std::set<OblastCode> to_show = {
        Lviv,
        Donetsk,
        Crimea
    };
    STATE.bounding_draw_mode = BoundingDrawMode::ALL;
    /*   for (Mesh* mesh : ukraine_map->meshes) {
           SetupBoundingBox(mesh, ukraine_map_model_index);
           oblasts.push_back(Oblast(mesh, static_cast<OblastCode>(i), "Oblast", 1.0));
           if (to_show.contains(oblasts[i].code)) {
               STATE.bounding_boxes_to_draw.insert(i);
               for (auto& v : oblasts[i].mesh->vertices) {
                   v.Color = { 1.0, 0.2, 0.2, 1.0 };
               }
               oblasts[i].mesh->UpdateBuffer();
           } else {
               for (auto& v : oblasts[i].mesh->vertices) {
                   v.Color = color;
               }
               oblasts[i].mesh->UpdateBuffer();
               color.r += 0.025;
               color.b += 0.025;
               color.a = 1.0;
           }
           i++;
       }*/

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
