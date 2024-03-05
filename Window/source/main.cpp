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
        window.onBeginOfTheLoop();

        glfwPollEvents();

        if (frame_counter++ > 144 * 1) {
            bool was_reloaded = HotReloadGameCode(&game);
            if (was_reloaded) game.GameInit(&window);
            frame_counter = 0;
        }

        game.GameUpdateAndRender(&window);

        glfwSwapBuffers(window.window);
    }

    glfwTerminate();
    return 0;
}
