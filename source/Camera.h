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
const float YAW = -90.0f;
const float PITCH = 0.0f;
//const float SPEED = 2.5f * 20;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.4f;
const float ZOOM = 45.0f;

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

    float yaw = YAW;
    float pitch = PITCH;

    float speed = SPEED;
    float sensitivity = SENSITIVITY;
    float zoom = ZOOM;
    float zoomSpeed = 1.2f;

    float lastX;
    float lastY;
    bool firstMouse = true;

    Camera(v3 position = v3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH, v3 up = v3(0.0f, 1.0f, 0.0f))
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
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
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

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        if (status == Cam::DISABLED)
            return;

        float velocity = speed * deltaTime;

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

    void ProcessMouseMovement(float xpos, float ypos, GLboolean constrainPitch = true)
    {
        if (status == Cam::DISABLED)
            return;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

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

    void ProcessMouseScroll(float yoffset)
    {
        if (status == Cam::DISABLED)
            return;

        zoom -= (float)yoffset * zoomSpeed;
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