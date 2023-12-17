#include "Camera.h"
#include "Constants.h"
#include "EntityManager.h"
#include "GameState.h"
#include "Model.h"
#include "Renderer.h"
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
    STATE.resources = ResourceManager();

    if (!STATE.window.Init()) {
        return -1;
    }

    WindowManager* window = &STATE.window;
    Camera* camera = &STATE.window.camera;

    glViewport(0, 0, window->screen_size.x, window->screen_size.y);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    Shader* texture_shader = RM.LoadShader("texture.vert", "texture.frag", "texture");
    Shader* objects_shader = RM.LoadShader("object.vert", "object.frag", "object");

    stbi_set_flip_vertically_on_load(true);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model ukraine_map = Model("D:/Projects/Dread/assets/map/map2.obj");
    // Model backpack = Model("D:/Projects/Dread/assets/backpack/backpack.obj");

    while (!STATE.window.IsClosing()) {
        STATE.window.onBeginOfTheLoop();
        glfwPollEvents();

        processInput(camera, window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        E.Update();
        R.Draw();

        objects_shader->Use();
        m4 projection = glm::perspective(glm::radians(camera->zoom), (float)STATE.window.screen_size.x / (float)STATE.window.screen_size.y, 0.1f, 100.0f);
        m4 view = camera->GetViewMatrix();
        objects_shader->setMat4("projection", projection);
        objects_shader->setMat4("view", view);
        objects_shader->setVec4("color", v4(0.2, 0.5, 1.0, 1.0));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        objects_shader->setMat4("model", model);

        ukraine_map.Draw(objects_shader);

        printf("Camera: %f %f %f | %f %f\r", camera->position.x, camera->position.y, camera->position.z, camera->yaw, camera->pitch);

        glfwSwapBuffers(window->window);
    }

    glfwTerminate();
    return 0;
}
