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
    CameraRight
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f * 5;
const float SENSITIVITY = 0.4f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera Attributes
    v3 position = { 0.0, 0.0, 0.0 };
    v3 front = { 0.0, 0.0, 0.0 };
    v3 up = { 0.0, 0.0, 0.0 };
    v3 right = { 0.0, 0.0, 0.0 };
    v3 world_up = { 0.0, 0.0, 0.0 };

    // euler Angles
    float yaw = YAW;
    float pitch = PITCH;
    // camera options
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
        updateCameraVectors();
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
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = speed * deltaTime;
        if (direction == CameraForward)
            position += front * velocity;
        if (direction == CameraBackward)
            position -= front * velocity;
        if (direction == CameraLeft)
            position -= right * velocity;
        if (direction == CameraRight)
            position += right * velocity;
    }

    void ProcessMouseMovement(float xpos, float ypos, GLboolean constrainPitch = true)
    {
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

        printf("%f %f\r", pitch, yaw);

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        zoom -= (float)yoffset * zoomSpeed;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        v3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, world_up)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
};
#endif