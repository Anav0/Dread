#include "WindowManager.h"

void WindowManager::onBeginOfTheLoop()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;

	frame_start_time = std::chrono::high_resolution_clock::now();

    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    buttonType = NONE;
    buttonAction = NEITHER;
}

void WindowManager::onMouseClicked(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        buttonType = RIGHT;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
        buttonType = LEFT;

    if (action == GLFW_PRESS)
        buttonAction = PRESSED;

    if (action == GLFW_RELEASE)
        buttonAction = RELEASED;
}

bool WindowManager::Init()
{
    screen_size.x = 1920;
    screen_size.y = 1080;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_size.x, screen_size.y, "Gwent", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, 200, 200);
    glfwSetWindowUserPointer(window, this);

    auto func = [](GLFWwindow* w, int btn, int action, int mods) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->onMouseClicked(w, btn, action, mods);
    };

    glfwSetMouseButtonCallback(window, func);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

bool WindowManager::IsClosing()
{
    return glfwWindowShouldClose(window);
}
