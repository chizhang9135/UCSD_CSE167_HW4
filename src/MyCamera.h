#ifndef MYCAMERA_H
#define MYCAMERA_H


#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"


class MyCamera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    const float cameraSpeedFactor = 150.0f;
    const float sensitivity = 0.1f;  // Adjust sensitivity as needed

    // Constructor with vectors
    MyCamera(float screenWidth, float screenHeight, glm::vec3 position, glm::vec3 up, glm::vec3 front);

    // Resets the camera to its initial state
    void Reset();

    // Processes input received from any keyboard-like input system
    void ProcessKeyboard(GLFWwindow *window, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    glm::vec3 InitialPosition;
    glm::vec3 InitialFront;
    glm::vec3 InitialUp;

    float screenWidth;
    float screenHeight;

    float lastX;
    float lastY;
    bool firstMouse;
    float yaw;
    float pitch;

    void updateCameraVectors();
};

#endif // MYCAMERA_H
