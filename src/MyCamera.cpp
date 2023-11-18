#include "MyCamera.h"


MyCamera::MyCamera(const hw3::Camera& camera) {
    glm::mat4 camToWorld = convertToGLMmat4(camera.cam_to_world);

    Position = glm::vec3(camToWorld[3]);
    Front = -glm::vec3(camToWorld[2]); // Forward vector is negative Z
    Up = glm::vec3(camToWorld[1]); // Up vector is Y axis
    WorldUp = Up;

    yaw = glm::degrees(atan2(Front.z, Front.x));
    pitch = glm::degrees(asin(Front.y));

    lastX = camera.resolution.x / 2.0f;
    lastY = camera.resolution.y / 2.0f;
    firstMouse = true;

    InitialPosition = Position;
    InitialFront = Front;
    InitialUp = Up;
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


