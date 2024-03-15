#include "WindowManager.h"
#include "GLFW/glfw3.h"
#include "Renderer.h"

void WindowManager::onBeginOfTheLoop()
{
    f32 currentFrame = static_cast<f32>(glfwGetTime());
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;

    frame_start_time = std::chrono::high_resolution_clock::now();

    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    mouse_y = screen_size.y - mouse_y;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    buttonType = MouseButton::NONE;
    buttonAction = MouseAction::NEITHER;
}

void WindowManager::onMouseClicked(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        buttonType = MouseButton::RIGHT;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
        buttonType = MouseButton::LEFT;

    if (action == GLFW_PRESS)
        buttonAction = MouseAction::PRESSED;

    if (action == GLFW_RELEASE)
        buttonAction = MouseAction::RELEASED;
}

bool WindowManager::Init()
{
	camera = Camera(v3(-1.2f, 15.0f, 13.0f), -83.0f, -50.0f);

    screen_size.x = 1920;
    screen_size.y = 1080;

    camera.lastX = screen_size.x / 2.0f;
    camera.lastY = screen_size.y / 2.0f;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

		//TODO: change after game is done
		i32 number_of_monitors = 0;
		auto monitors = glfwGetMonitors(&number_of_monitors);
		auto monitor = monitors[0];
		if(number_of_monitors > 1) monitor = monitors[1];

		i32 x;
		i32 y;
		glfwGetMonitorPos(monitor, &x, &y);
    window = glfwCreateWindow(screen_size.x, screen_size.y, "Dread", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

		glfwSetWindowPos(window, x + 500, y + 200);
    glfwSetWindowUserPointer(window, this);

    auto mouse_click_lambda = [](GLFWwindow* w, int btn, int action, int mods) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->onMouseClicked(w, btn, action, mods);
    };

    auto mouse_move_lambda = [](GLFWwindow* w, f64 xposIn, f64 yposIn) {
        f32 x = static_cast<f32>(xposIn);
        f32 y = static_cast<f32>(yposIn);
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->camera.ProcessMouseMovement(x, y);
    };

    auto scroll_lambda = [](GLFWwindow* w, f64 xoffset, f64 yoffset) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->camera.ProcessMouseScroll(static_cast<f32>(yoffset));
    };

    auto keypress_lambda = [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        static_cast<WindowManager*>(glfwGetWindowUserPointer(w))->ProcessInput();
    };

	auto framebuffer_size_lambda = [](GLFWwindow* w, int width, int height)
	{
		glViewport(0, 0, width, height);
        WindowManager* wm = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
        wm->Resize(width, height);
        R.UpdateProjection(wm->camera, {width, height});
	};

    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouse_click_lambda);
    glfwSetCursorPosCallback(window, mouse_move_lambda);
    glfwSetScrollCallback(window, scroll_lambda);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_lambda);
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

void WindowManager::Resize(u32 width, u32 height)
{
    screen_size.x = width;
    screen_size.y = height;
}

void WindowManager::ProcessInput()
{
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (camera.status == CameraStatus::DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.status = CameraStatus::ENABLED;
            return;
        }
        if (camera.status == CameraStatus::ENABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera.status = CameraStatus::DISABLED;
            return;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        R.mode = RenderMode::NORMAL;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        R.mode = RenderMode::WIREFRAME;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDir::Forward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDir::Backward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDir::Left, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDir::Right, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDir::Down, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraDir::Up, delta_time);
}
