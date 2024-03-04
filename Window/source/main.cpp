#include <algorithm>
#include <set>

#include "Engine/WindowManager.h"
#include <Engine/HotReload.h>

int main(int argc, char* argv[])
{
    WindowManager window;
    if (!window.Init()) {
        printf("ERROR: Failed to initialize WindowManager\n");
        return -1;
    }

    GameCode game = LoadGameCode();
    game.GameInit(&window);

    u32 frame_counter = 0;
    while (!window.IsClosing()) {
        glfwPollEvents();

        window.onBeginOfTheLoop();

        if (frame_counter++ > 144) {
            HotReloadGameCode(&game);
            frame_counter = 0;
        }

        game.GameUpdateAndRender(&window);
        glfwSwapBuffers(window.window);
    }

    glfwTerminate();
    return 0;
}
