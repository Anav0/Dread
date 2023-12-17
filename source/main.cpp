#include "GameState.h"
#include "Renderer.h"
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char* argv[])
{
    STATE = GameState();
    STATE.window = WindowManager();
    STATE.resources = ResourceManager();

    if (!STATE.window.Init()) {
        return -1;
    }

    Model model = Model("D:/Projects/Dread/assets/map.obj");
  
    glfwTerminate();
    return 0;
}
