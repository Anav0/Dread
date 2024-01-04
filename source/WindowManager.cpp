#include "WindowManager.h"
#include "GameState.h"

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

    camera.lastX = screen_size.x / 2.0f;
    camera.lastY = screen_size.y / 2.0f;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_size.x, screen_size.y, "Dread", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, 400, 200);
    glfwSetWindowUserPointer(window, this);

    auto mouse_click_lambda = [](GLFWwindow* w, int btn, int action, int mods) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->onMouseClicked(w, btn, action, mods);
    };

    auto mouse_move_lambda = [](GLFWwindow* w, double xposIn, double yposIn) {
        float x = static_cast<float>(xposIn);
        float y = static_cast<float>(yposIn);
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->camera.ProcessMouseMovement(x, y);
    };

    auto scroll_lambda = [](GLFWwindow* w, double xoffset, double yoffset) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->camera.ProcessMouseScroll(static_cast<float>(yoffset));
    };

    auto keypress_lambda = [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->ProcessInput();
    };

    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_click_lambda);
    glfwSetCursorPosCallback(window, mouse_move_lambda);
    glfwSetScrollCallback(window, scroll_lambda);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keypress_lambda);

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

void WindowManager::ProcessInput()
{
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (camera.status == Cam::DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.status = Cam::ENABLED;
            return;
        }
        if (camera.status == Cam::ENABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera.status = Cam::DISABLED;
            return;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        STATE.mode = RenderMode::NORMAL;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        STATE.mode = RenderMode::WIREFRAME;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        STATE.bounding_draw_mode = BoundingDrawMode::ALL;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraForward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraBackward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraLeft, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraRight, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDown, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraUp, delta_time);
}
