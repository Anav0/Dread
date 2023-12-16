#define STB_IMAGE_IMPLEMENTATION

#include "GameState.h"
#include "Renderer.h"

int main(int argc, char* argv[])
{
    STATE = GameState();
    STATE.window = WindowManager();
    STATE.resources = ResourceManager();

    if (!STATE.window.Init()) {
        return -1;
    }

  
    glfwTerminate();
    return 0;
}
