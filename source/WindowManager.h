#pragma once

#include <iostream>

// GLWF MUST ALWAYS BE INCLUDED AFTER GLAD
#include "Constants.h"
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Misc.h"
#include "TimeManager.h"

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

enum MouseButton {
    NONE,
    LEFT,
    RIGHT
};

enum MouseAction {
    NEITHER,
    PRESSED,
    RELEASED
};

class WindowManager {
public:
    GLFWwindow* window;
    Camera camera; // TODO: temporary
    v2 screen_size;

    f64 mouse_x, mouse_y;
    MouseButton buttonType;
    MouseAction buttonAction;

    TimeManager time_manager;

    f32 delta_time, last_frame;
    high_resolution_clock::time_point frame_start_time;

    bool Init();
    bool IsClosing();
    void onMouseClicked(GLFWwindow* window, int button, int action, int mods);
    void onBeginOfTheLoop();
    void ProcessInput();
};
