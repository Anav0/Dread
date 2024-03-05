#include <algorithm>
#include <set>

#include "Engine/WindowManager.h"
#include <Engine/HotReload.h>
#include <Game/GameState.h>

/*

    Window should keep game state, and only functions that change the behavior of the game should be hot
    reloaded
*/

int main(int argc, char* argv[])
{
    WindowManager window;
    GameState* current_game_state = nullptr;

    if (!window.Init()) {
        printf("ERROR: Failed to initialize WindowManager\n");
        return -1;
    }

    GameCode game = LoadGameCode();
    current_game_state = game.GameInit(&window);

    u32 frame_counter = 0;
    while (!window.IsClosing()) {
        window.onBeginOfTheLoop();

        glfwPollEvents();

        if (frame_counter++ > 144 * 1) {
            if (GameCodeChanged(&game)) {
                GameState state_copy = *current_game_state;
                UnloadGameCode(&game);
                printf("\n\n\n\n%i\n\n\n\n", state_copy.current_turn);
                game = LoadGameCode();
                current_game_state = game.GameInitEx(state_copy, &window);
            }
            frame_counter = 0;
        }

        game.GameUpdateAndRender(&window);

        glfwSwapBuffers(window.window);
    }

    glfwTerminate();
    return 0;
}
