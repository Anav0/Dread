#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    CameraForward,
    CameraBackward,
    CameraLeft,
    CameraRight,
    CameraUp,
    CameraDown,
};

// Default camera values
const f32 YAW = -90.0f;
const f32 PITCH = 0.0f;
//const f32 SPEED = 2.5f * 20;
const f32 SPEED = 2.5f;
const f32 SENSITIVITY = 0.4f;
const f32 ZOOM = 45.0f;

namespace Cam {
enum CameraStatus {
    DISABLED = 0,
    ENABLED = 1,
};
}

class Camera {
public:
    Cam::CameraStatus status = Cam::DISABLED;

    v3 position = { 0.0, 0.0, 0.0 };
    v3 front = { 0.0, 0.0, 0.0 };
    v3 up = { 0.0, 0.0, 0.0 };
    v3 right = { 0.0, 0.0, 0.0 };
    v3 world_up = { 0.0, 0.0, 0.0 };

    f32 yaw = YAW;
    f32 pitch = PITCH;

    f32 speed = SPEED;
    f32 sensitivity = SENSITIVITY;
    f32 zoom = ZOOM;
    f32 zoomSpeed = 1.2f;

    f32 lastX;
    f32 lastY;
    bool firstMouse = true;

    Camera(v3 position = v3(0.0f, 0.0f, 0.0f), f32 yaw = YAW, f32 pitch = PITCH, v3 up = v3(0.0f, 1.0f, 0.0f))
        : front(v3(0.0f, 0.0f, -1.0f))
        , speed(SPEED)
        , sensitivity(SENSITIVITY)
        , zoom(ZOOM)
    {
        this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        UpdateCameraVectors();
    }
    Camera(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 yaw, f32 pitch)
        : front(v3(0.0f, 0.0f, -1.0f))
        , speed(SPEED)
        , sensitivity(SENSITIVITY)
        , zoom(ZOOM)
    {
        this->position = v3(posX, posY, posZ);
        this->world_up = v3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    void ProcessKeyboard(Camera_Movement direction, f32 deltaTime)
    {
        if (status == Cam::DISABLED)
            return;

        f32 velocity = speed * deltaTime;

        if (direction == CameraForward)
            position += front * velocity;
        if (direction == CameraBackward)
            position -= front * velocity;
        if (direction == CameraLeft)
            position -= right * velocity;
        if (direction == CameraRight)
            position += right * velocity;
        if (direction == CameraUp)
            position += up * velocity;
        if (direction == CameraDown)
            position -= up * velocity;


        UpdateCameraVectors();
    }

    void ProcessMouseMovement(f32 xpos, f32 ypos, GLboolean constrainPitch = true)
    {
        if (status == Cam::DISABLED)
            return;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        f32 xoffset = xpos - lastX;
        f32 yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        UpdateCameraVectors();
    }

    void ProcessMouseScroll(f32 yoffset)
    {
        if (status == Cam::DISABLED)
            return;

        zoom -= (f32)yoffset * zoomSpeed;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;

        UpdateCameraVectors();
    }

private:
    void UpdateCameraVectors()
    {
        v3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);
        right = glm::normalize(glm::cross(front, world_up));
        up = glm::normalize(glm::cross(right, front));
    }
};
#endif