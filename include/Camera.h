#pragma once

#include "global.h"
#include "Matrix.h"

namespace psi {

class Camera {
public:

    float mouseSensitivity = 0.1f;
    float cameraSpeed = 5.f;

    Vec3f front;
    Vec3f up;
    Vec3f right;

    Vec3f pos;
    Vec3f worldUp;
    float yaw = 0.f;
    float pitch = 0.f;

    Camera() {}
    Camera(const Vec3f& Pos, const Vec3f& Up, float Yaw, float Pitch) : 
        pos(Pos), worldUp(Up.normalized()), yaw(Yaw), pitch(Pitch){ 
        UpdateAxis();
    }

    Mat4f ViewMatrix() {
        return Matrix::LookAt(pos, pos + front, worldUp);
    }

    float lastX = 0.f;
    float lastY = 0.f;
    bool isMouseCaptured = false;
    void RotateByMouse(float xpos, float ypos) {
        if (!isMouseCaptured) {
            lastX = xpos;
            lastY = ypos;
            isMouseCaptured = true;
            //pitch = asin(cameraFront.y());
            //yaw = -atan2(cameraFront.x(), cameraFront.z());
        }

        float deltaX = lastX - xpos;
        float deltaY = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        yaw += deltaX * mouseSensitivity;
        pitch += deltaY * mouseSensitivity;

        if (pitch > 89.9f) {
            pitch = 89.9f;
        }
        else if (pitch < -89.9f) {
            pitch = -89.9f;
        }
        UpdateAxis();
    }

    enum class Direction {
        left, right, up, down, forward, backward
    };
    void Move(Direction dir, float deltaTime) {
        float displacement = cameraSpeed * deltaTime;
        switch (dir)
        {
        case Direction::left:
            pos -= right * displacement;
            break;
        case Direction::right:
            pos += right * displacement;
            break;
        case Direction::up:
            pos += worldUp * displacement;
            break;
        case Direction::down:
            pos -= worldUp * displacement;
            break;
        case Direction::forward:
            pos += front * displacement;
            break;
        case Direction::backward:
            pos -= front * displacement;
            break;
        default:
            break;
        }
    }

private:
    void UpdateAxis() {
        front.x() = sin(deg2rad(yaw)) * cos(deg2rad(pitch));
        front.y() = sin(deg2rad(pitch));
        front.z() = cos(deg2rad(yaw)) * cos(deg2rad(pitch));

        right = front.cross(worldUp).normalized();
        up = right.cross(front).normalized();
    }
};

}