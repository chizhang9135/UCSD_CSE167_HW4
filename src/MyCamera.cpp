#include "MyCamera.h"


MyCamera::MyCamera(float screenWidth, float screenHeight, glm::vec3 position, glm::vec3 up, glm::vec3 front)
        : Position(position), Front(front), WorldUp(up) {
    InitialPosition = position;
    InitialFront = front;
    InitialUp = up;
    lastX = screenWidth / 2.0f;
    lastY = screenHeight / 2.0f;
    firstMouse = true;
    yaw = -90.0f;
    pitch = 0.0f;
    updateCameraVectors();
}

void MyCamera::Reset() {
    Position = InitialPosition;
    Front = InitialFront;
    Up = InitialUp;
    updateCameraVectors();
}


void MyCamera::ProcessKeyboard(GLFWwindow *window, float deltaTime) {
    float velocity = cameraSpeedFactor * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += Front * velocity;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= Front * velocity;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= Right * velocity;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += Right * velocity;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        Reset();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

glm::mat4 MyCamera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void MyCamera::updateCameraVectors() {
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void MyCamera::ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Forward and backward movement
    Position += Front * yoffset;

    // Right and left movement
    Position += Right * xoffset;
}


