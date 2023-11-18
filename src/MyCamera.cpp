#include "MyCamera.h"


MyCamera::MyCamera(float screenWidth, float screenHeight, glm::vec3 position, glm::vec3 up, glm::vec3 front)
        : Position(position), Front(front), WorldUp(up) {
    yaw = glm::degrees(atan2(Front.z, Front.x));
    pitch = glm::degrees(asin(Front.y));

    lastX = screenWidth / 2.0f;
    lastY = screenHeight / 2.0f;
    firstMouse = true;

    InitialPosition = position;
    InitialFront = front;
    InitialUp = up;
    InitialYaw = yaw;
    InitialPitch = pitch;

    updateCameraVectors();
}

void MyCamera::Reset() {
    Position = InitialPosition;
    Front = InitialFront;
    Up = InitialUp;
    yaw = InitialYaw;
    pitch = InitialPitch;
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
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void MyCamera::ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;


    updateCameraVectors();
}

