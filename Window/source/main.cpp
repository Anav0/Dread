#include <algorithm>
#include <set>

#include "Engine/Gui.h"
#include "Engine/HotReload.h"
#include "Engine/ResourceManager.h"
#include "Engine/WindowManager.h"

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
    game.GameInit(&window);

#if DEBUG_LINES
    std::vector<Line> lines;
#endif

    u64 frame_counter = 0;
    MouseInfo info {};
    while (!window.IsClosing()) {
        window.onBeginOfTheLoop();

        info.action = window.buttonAction;
        info.type = window.buttonType;
        info.pos.x = window.mouse_x;
        info.pos.y = window.mouse_y;

        UI.onFrameBegin(info);

        glfwPollEvents();

        if (frame_counter++ > 144) {
            HotReloadGameCode(&game);
            RM.HotReloadShaders();
            frame_counter = 0;
        }
        game.GameUpdateAndRender(&window);

        glfwSwapBuffers(window.window);
    }

    glfwTerminate();
    return 0;
}
