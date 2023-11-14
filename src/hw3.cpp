#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"
#include "hw3_scenes.h"

using namespace hw3;

void printMatrix(const glm::mat4 m) {
    //print prompt in blue
    std::cout << "\033[1;34m";
    std::cout << "Matrix: " << std::endl;
    //print in color of yellow
    std::cout << "\033[1;33m";
    for (int i = 0; i < 4; i++) {
        std::cout << "[";
        for (int j = 0; j < 4; j++) {
            std::cout << m[i][j] << " ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "\033[0m";
}
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Function to check shader compilation/linking errors.
void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

/**
 * @brief Compile a shader
 * @param shader The shader to compile
 */
void compileShader(GLuint shader) {
    glCompileShader(shader);
    checkCompileErrors(shader, shader == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
}

/**
 * @brief Build and compile a shader program
 * @param vertexShaderSource The vertex shader source code
 * @param fragmentShaderSource The fragment shader source code
 * @return The shader program
 */
GLuint buildAndCompileShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    compileShader(vertexShader);

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    compileShader(fragmentShader);

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/**
 * @brief Create a transformation matrix
 * @param screenWidth The width of the window
 * @param screenHeight The height of the window
 * @return The transformation matrix
 */
glm::mat4 createTransformationMatrix(int screenWidth, int screenHeight, float speed) {
    glm::mat4 transform = glm::mat4(1.0f);
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    transform = glm::rotate(transform, (float)glfwGetTime() * speed, glm::vec3(0.0, 0.0, 1.0));
    transform = projection * transform;
    return transform;
}

/**
 * @brief HW 3.1: Render a OpenGL window
 * @param params Unused
 */
void hw_3_1(const std::vector<std::string>& params) {
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW_3_1", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Process input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

/**
 * @brief HW 3.2: Render a triangle that rotates in window using OpenGL
 * @param params Unused
 */
void hw_3_2(const std::vector<std::string> &params) {
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "uniform mat4 transform;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = transform * vec4(aPos, 1.0);\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(0.9f, 0.4f, 0.4f, 1.0f);\n"
                                       "}\n\0";
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Uncomment this statement for macOS
#endif

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW_3_2", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Build and compile our shader program
    GLuint shaderProgram = buildAndCompileShaderProgram(vertexShaderSource, fragmentShaderSource);


    float vertices[] = {
            // Right triangle vertices
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Rendering commands
        glClearColor(0.22f, 0.33f, 0.54f, 0.98f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate shader program
        glUseProgram(shaderProgram);

        // Create transformations
        glm::mat4 transform = createTransformationMatrix(SCR_WIDTH, SCR_HEIGHT,1.5);

        // Get the transformation uniform location and set the transformation matrix
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        // Render the triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // GLFW: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return;
}

/**
 * @brief Convert a Matrix4x4f to a glm::mat4
 * @param m The Matrix4x4f to convert
 * @return The glm::mat4
 */
glm::mat4 convertToGLMmat4(const Matrix4x4f m) {
    glm::mat4 glmMatrix;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            glmMatrix[j][i] = m(i, j); // Note: glm::mat4 is also column-major
        }
    }
    return glmMatrix;
}

/**
 * @brief HW 3.3: Render a scene
 * @param params
 */
void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }
    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    unsigned int SCR_WIDTH = scene.camera.resolution.x;
    unsigned int SCR_HEIGHT = scene.camera.resolution.y;
    float background[3] = {scene.background.x, scene.background.y, scene.background.z};

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 Colors;\n"
                                     "uniform mat4 model_matrix;\n"
                                     "uniform mat4 view_matrix;\n"
                                     "uniform mat4 projection_matrix;\n"
                                     "out vec3 ColorsVector;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = projection_matrix * view_matrix * model_matrix  * vec4(aPos, 1.0);\n"
                                     "   ColorsVector = Colors;\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "in vec3 ColorsVector;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(ColorsVector, 1.0f);\n"
                                       "}\n\0";

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW_3_3", NULL, NULL);

    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    // Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }



    // Build and compile our shader program

    GLuint shaderProgram = buildAndCompileShaderProgram(vertexShaderSource, fragmentShaderSource);

    // VAOS and VBOS Vectors
    std::vector<GLuint> VAOs;
    std::vector<GLuint> VBOs_Vertex;
    std::vector<GLuint> VBOs_Color;

    // loop over mesh and create VAOs and VBOs
    for (auto &mesh : scene.meshes) {
        // Create VAO
        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        VAOs.push_back(VAO);

        // Create VBO for vertices
        GLuint VBO_Vertex;
        glGenBuffers(1, &VBO_Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertex);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        VBOs_Vertex.push_back(VBO_Vertex);

        // Create VBO for colors
        GLuint VBO_Color;
        glGenBuffers(1, &VBO_Color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        VBOs_Color.push_back(VBO_Color);

        // Set vertex attribute pointers
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertex);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        // Unbind VAO
        glBindVertexArray(0);
    }


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        //enable depth test

        // Rendering commands
        glClearColor(background[0], background[1], background[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        glUseProgram(shaderProgram);

        float aspectRatio = SCR_WIDTH/SCR_HEIGHT; // Aspect ratio of the viewport (width / height)
        float scale = scene.camera.s; // Scale factor, often computed as 1 / tan(fov / 2)
        float zNear = scene.camera.z_near; // Distance to the near clipping plane
        float zFar = scene.camera.z_far; // Distance to the far clipping plane
        glm::mat4 projection_matrix;
        // initialize projection matrix with zeros
        projection_matrix = glm::mat4(0.0f);
        projection_matrix[0][0] = 1.0f / (aspectRatio * scale);
        projection_matrix[1][1] = 1.0f / scale;
        projection_matrix[2][2] = -zFar / (zFar - zNear);
        projection_matrix[2][3] = -(zFar * zNear) / (zFar - zNear);
        projection_matrix[3][2] = -1.0f;

        printMatrix(projection_matrix);
        unsigned int projection_matrixLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
        glUniformMatrix4fv(projection_matrixLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Calculate view matrix once
        glm::mat4 view_matrix = convertToGLMmat4(inverse(scene.camera.cam_to_world));
        unsigned int view_matrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
        glUniformMatrix4fv(view_matrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

        // Render each mesh
        for (int i = 0; i < scene.meshes.size(); i++) {
            glm::mat4 model_matrix = convertToGLMmat4(scene.meshes[i].model_matrix);
            unsigned int model_matrixLoc = glGetUniformLocation(shaderProgram, "model_matrix");
            glUniformMatrix4fv(model_matrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

            glBindVertexArray(VAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, scene.meshes[i].vertices.size());
        }


        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // De-allocate all resources
    for (int i = 0; i < VAOs.size(); i++) {
        glDeleteVertexArrays(1, &VAOs[i]);
        glDeleteBuffers(1, &VBOs_Vertex[i]);
        glDeleteBuffers(1, &VBOs_Color[i]);
    }

    glDeleteProgram(shaderProgram);

    // GLFW: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();

}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
