#include "OpenGLHelper.h"


void OpenGLHelper::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void OpenGLHelper::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint OpenGLHelper::CompileShader(GLenum type, const char* shaderSource) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint OpenGLHelper::createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

glm::mat4 OpenGLHelper::createTransformationMatrix(int screenWidth, int screenHeight, float speed) {
    glm::mat4 transform = glm::mat4(1.0f);
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    transform = glm::rotate(transform, (float)glfwGetTime() * speed, glm::vec3(0.0, 0.0, 1.0));
    transform = projection * transform;
    return transform;
}

glm::mat4 OpenGLHelper::convertToGLMmat4(const Matrix4x4f& m) {
    glm::mat4 glmMatrix;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            glmMatrix[col][row] = m(row, col);
        }
    }
    return glmMatrix;
}
//
// Created by Chi Zhang on 11/16/23.
//

#include "OpenGLHelper.h"
