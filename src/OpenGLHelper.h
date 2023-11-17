#ifndef OPENGLHELPER_H
#define OPENGLHELPER_H

#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"
#include "hw3_scenes.h"
#include "MyCamera.h"


namespace OpenGLHelper {
    void processInput(GLFWwindow *window);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    GLuint CompileShader(GLenum type, const char* shaderSource);
    GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    glm::mat4 createTransformationMatrix(int screenWidth, int screenHeight, float speed);
    glm::mat4 convertToGLMmat4(const Matrix4x4f& m);
    // other declarations...
}

#endif // OPENGLHELPER_H
