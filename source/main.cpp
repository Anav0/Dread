#include "GameState.h"
#include "Model.h"
#include "Renderer.h"
#include "EntityManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Order is im

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
    STATE = GameState();
    STATE.window = WindowManager();
    STATE.resources = ResourceManager();

    if (!STATE.window.Init()) {
        return -1;
    }

    auto WINDOW_MGR = &STATE.window;

    glViewport(0, 0, WINDOW_MGR->screen_size.x, WINDOW_MGR->screen_size.y);

    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDebugMessageCallback(MessageCallback, 0);

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Shader* texture_shader = RM.LoadShader("texture.vert", "texture.frag", "texture");

    m4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_MGR->screen_size.x),
        static_cast<float>(WINDOW_MGR->screen_size.y), 0.0f, -1.0f, 1.0f);

    float delta_time = 0, last_frame = 0;

    Model model = Model("D:/Projects/Dread/assets/map.obj");

    while (!STATE.window.IsClosing()) {
        STATE.window.onBeginOfTheLoop();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        E.Update();
        R.Draw();

        glfwSwapBuffers(WINDOW_MGR->window);
    }

    glfwTerminate();
    return 0;
}
