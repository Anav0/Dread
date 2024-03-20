#pragma once

#include <iostream>

// GLWF MUST ALWAYS BE INCLUDED AFTER GLAD
#include "Constants.h"
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Misc.h"
#include "TimeManager.h"

enum class MouseButton {
    NONE,
    LEFT,
    RIGHT
};

enum class MouseAction {
    NEITHER,
    PRESSED,
    RELEASED
};

class WindowManager {
public:
    GLFWwindow* window;
    Camera camera;
    v2 screen_size;

    f64 mouse_x, mouse_y;
    MouseButton buttonType;
    MouseAction buttonAction;

    TimeManager time_manager;

    f32 delta_time, last_frame, time_since_start_s;
    high_resolution_clock::time_point frame_start_time;

    bool Init();
    bool IsClosing();
    void Resize(u32 w, u32 h);
    void onMouseClicked(GLFWwindow* window, int button, int action, int mods);
    void onBeginOfTheLoop();
    void ProcessInput();
};
