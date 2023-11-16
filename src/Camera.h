#ifndef CAMERA_H
#define CAMERA_H

#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f));

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

private:
    void updateCameraVectors();
};

#endif //CAMERA_H
