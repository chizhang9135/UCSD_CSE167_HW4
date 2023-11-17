#ifndef MYCAMERA_H
#define MYCAMERA_H

#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"
#include "hw3_scenes.h"

class MyCamera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Constructor with vectors
    MyCamera(glm::vec3 position, glm::vec3 up, glm::vec3 front);

    // Resets the camera to its initial state
    void Reset();

    // Processes input received from any keyboard-like input system
    void ProcessKeyboard(GLFWwindow *window, float deltaTime);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

private:
    // Initial settings
    glm::vec3 InitialPosition;
    glm::vec3 InitialFront;
    glm::vec3 InitialUp;

    void updateCameraVectors();
};

#endif // MYCAMERA_H
