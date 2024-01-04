#include "Camera.h"
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

#define STB_IMAGE_IMPLEMENTATION
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

void processInput(Camera* camera, WindowManager* mgr)
{
    auto window = mgr->window;
    float delta_time = mgr->delta_time;

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (camera->status == Cam::DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera->status = Cam::ENABLED;
            return;
        }
        if (camera->status == Cam::ENABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera->status = Cam::DISABLED;
            return;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(CameraForward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(CameraBackward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(CameraLeft, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(CameraRight, delta_time);
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

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

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

    // Model ukraine_map = Model("D:/Projects/Dread/assets/map/map2.obj");
    // std::vector<Oblast> oblasts;
    // int i = 0;
    // for (Mesh& mesh : ukraine_map.meshes) {
    //     oblasts.push_back(Oblast(&mesh, static_cast<OblastCode>(i), "Oblast", 1.0));
    //     if (oblasts[i].code == Luhansk || oblasts[i].code == Donetsk) {

    //        for (auto& v : oblasts[i].mesh->vertices) {
    //            v.Color = { 1.0, 0.2, 0.2, 1.0 };
    //        }
    //        oblasts[i].mesh->UpdateBuffer();
    //    }
    //    i++;
    //}

    /*Model* back = RM.LoadModel("backpack/backpack.obj", "backpack");
    R.models.push_back(back);*/

    //Model* couch = RM.LoadModel("couch/couch.obj", "couch");
    //R.models.push_back(couch);

    Model* map = RM.LoadModel("map/map2.obj", "map");
    R.models.push_back(map);


    // Model backpack = Model("D:/Projects/Dread/assets/backpack/backpack.obj");
    // Model sphere = Model("D:/Projects/Dread/assets/sphere/sphere.obj");

    m4 projection = glm::perspective(glm::radians(camera->zoom), (float)STATE.window.screen_size.x / (float)STATE.window.screen_size.y, 0.1f, 100.0f);
    m4 backpack_model = glm::translate(m4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));

    BoundingBox box = BoundingBox(v3(250, 250, 250), v3(0.0f, 10.0f, 3.0f));

    R.projection = projection;

    while (!STATE.window.IsClosing()) {
        STATE.window.onBeginOfTheLoop();
        glfwPollEvents();

        processInput(camera, window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        E.Update();

        R.Update();
        R.Draw();

        printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

        if (STATE.window.buttonAction == PRESSED && STATE.window.buttonType == LEFT) {
            auto ray_vector = GetRayFromEyes(&projection);
        }

        glfwSwapBuffers(window->window);
    }

    glfwTerminate();
    return 0;
}
