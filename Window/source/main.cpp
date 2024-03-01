#include <algorithm>
#include <set>

#include "Engine/WindowManager.h"
#include "Engine/HotReload.h"

#include <stb_image.h>

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
    WindowManager window;
    if (!window.Init()) {
        printf("ERROR: Failed to initialize WindowManager\n");
        return -1;
    }

    auto camera = &window.camera;

    glViewport(0, 0, window.screen_size.x, window.screen_size.y);

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

    GameCode game = LoadGameCode();

#if DEBUG_LINES
    std::vector<Line> lines;
#endif

    while (!window.IsClosing()) {
        window.onBeginOfTheLoop();
        glfwPollEvents();

        game.GameUpdateAndRender(&window);

        glfwSwapBuffers(window.window);
    }

    glfwTerminate();
    return 0;
}
