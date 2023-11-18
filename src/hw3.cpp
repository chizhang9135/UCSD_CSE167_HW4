#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "3rdparty/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/gtc/type_ptr.hpp"
#include "hw3_scenes.h"
#include "MyCamera.h"



using namespace hw3;


//<editor-fold desc="Global variables and helper functions">
/**
 * @brief Global variable for camera, create new camera if needed, remember to delete it
 */
MyCamera *camera = nullptr;

/**
 * @brief Global variable for extra credit
 */
const bool hw_3_4_extra = false;

/**
 * @brief Process input from keyboard
 * @param window GLFW window
 */
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/**
 * @brief Callback function for window resizing
 * @param window GLFW window
 * @param width New width
 * @param height New height
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/**
 * @brief Compile shader
 * @param type Shader type
 * @param shaderSource Shader source code
 * @return Shader ID
 */
GLuint CompileShader(GLenum type, const char* shaderSource) {
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

/**
 * @brief Create shader program
 * @param vertexShaderSource Vertex shader source code
 * @param fragmentShaderSource Fragment shader source code
 * @return Shader program ID
 */
GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
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

/**
 * @brief Create transformation matrix
 * @param screenWidth Screen width
 * @param screenHeight Screen height
 * @param speed Rotation speed
 * @return Transformation matrix
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
 * @brief Convert Matrix4x4f to glm::mat4
 * @param m Matrix4x4f
 * @return glm::mat4
 */
glm::mat4 convertToGLMmat4(const Matrix4x4f& m) {
    glm::mat4 glmMatrix;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            glmMatrix[col][row] = m(row, col);
        }
    }
    return glmMatrix;
}

/**
 * @brief Callback function for mouse movement
 * @param window GLFW window
 * @param xpos Mouse x position
 * @param ypos Mouse y position
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (camera) {
        if (camera->firstMouse) {
            camera->lastX = xpos;
            camera->lastY = ypos;
            camera->firstMouse = false;
        }

        float xoffset = xpos - camera->lastX;
        float yoffset = camera->lastY - ypos; // Reversed since y-coordinates go from bottom to top

        camera->lastX = xpos;
        camera->lastY = ypos;

        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

//</editor-fold>

//<editor-fold desc="HW 3.1 - 3.2: Window and rotating triangle">
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
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);


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
        glm::mat4 transform = createTransformationMatrix(SCR_WIDTH, SCR_HEIGHT,2.0f);

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
//</editor-fold>

/**
 * @brief HW 3.3: Render a scene
 * @param params json file
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
//
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "in vec3 ColorsVector;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(ColorsVector, 1.0f);\n"
                                       "}\n\0";

    glm::mat4 camToWorld = convertToGLMmat4(scene.camera.cam_to_world);
    glm::vec3 cameraPos = glm::vec3(camToWorld[3]);      // Camera position is in the 4th column
    glm::vec3 cameraFront = -glm::vec3(camToWorld[2]);   // Forward vector is negative Z (third column)
    glm::vec3 cameraUp = glm::vec3(camToWorld[1]);       // Up vector is the Y axis (second column)

    float deltaTime = 0.0f;                               // Time between current frame and last frame
    float lastFrame = 0.0f;                               // Time of last frame

    glm::vec3 initialCameraPos = cameraPos;
    glm::vec3 initialCameraFront = cameraFront;
    glm::vec3 initialCameraUp = cameraUp;

    camera = new MyCamera(SCR_WIDTH, SCR_HEIGHT, cameraPos, cameraUp, cameraFront);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW_3_3", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);




    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    std::vector<GLuint> VAOs;
    for (auto &mesh : scene.meshes) {
        unsigned int VAO, VBO_vertex, VBO_color, EBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        VAOs.push_back(VAO);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);




    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Measure time difference between frames
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera->ProcessKeyboard(window, deltaTime);

        glClearColor(background[0], background[1], background[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        glUseProgram(shaderProgram);

        float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
        float scale = scene.camera.s; // This is the scaling/film size parameter
        float zNear = scene.camera.z_near;
        float zFar = scene.camera.z_far;
        glm::mat4 projection_matrix = glm::mat4(
                1.0f / (aspectRatio * scale), 0.0f,                        0.0f,                            0.0f,
                0.0f,                          1.0f / scale,                0.0f,                            0.0f,
                0.0f,                          0.0f,                       -zFar / (zFar - zNear),          -1.0f,
                0.0f,                          0.0f,                       -(zFar * zNear) / (zFar - zNear), 0.0f
        );

        unsigned int projection_matrixLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
        glUniformMatrix4fv(projection_matrixLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Calculate view matrix once
        glm::mat4 view_matrix = camera->GetViewMatrix();  // Get the view matrix from MyCamera
        unsigned int view_matrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
        glUniformMatrix4fv(view_matrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

        // Render each mesh
        for (int i = 0; i < scene.meshes.size(); i++) {
            glm::mat4 model_matrix = convertToGLMmat4(scene.meshes[i].model_matrix);
            unsigned int model_matrixLoc = glGetUniformLocation(shaderProgram, "model_matrix");
            glUniformMatrix4fv(model_matrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            glBindVertexArray(VAOs[i]);
//            glDrawArrays(GL_TRIANGLES, 0, scene.meshes[i].vertices.size());
            glDrawElements(GL_TRIANGLES, scene.meshes[i].faces.size() * 3, GL_UNSIGNED_INT, 0);

        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i = 0; i < VAOs.size(); i++) {
        glDeleteVertexArrays(1, &VAOs[i]);
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    //free camera
    delete camera;

}

/**
 * @brief HW 3.4: Render a scene with lighting
 * @param params json file
 */
void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
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
                                     "layout (location = 2) in vec3 aNormal;\n"
                                     "uniform mat4 model_matrix;\n"
                                     "uniform mat4 view_matrix;\n"
                                     "uniform mat4 projection_matrix;\n"
                                     "out vec3 ColorsVector;\n"
                                     "out vec3 NormalVector;\n"
                                     "out vec3 FragPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = projection_matrix * view_matrix * model_matrix  * vec4(aPos, 1.0);\n"
                                     "   ColorsVector = Colors;\n"
                                     "   NormalVector = mat3(transpose(inverse(model_matrix))) * aNormal;\n"
                                     "   FragPos = vec3(model_matrix * vec4(aPos, 1.0));\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "in vec3 ColorsVector;\n"
                                       "in vec3 NormalVector;\n"
                                       "in vec3 FragPos;\n"
                                       "\n"
                                       "uniform vec3 lightPos;\n"
                                       "uniform vec3 lightColor;\n"
                                       "uniform vec3 viewPos; // Camera/view position\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    // Ambient lighting\n"
                                       "    float ambientStrength = 0.1;\n"
                                       "    vec3 ambient = ambientStrength * lightColor;\n"
                                       "\n"
                                       "    // Diffuse lighting\n"
                                       "    vec3 norm = normalize(NormalVector);\n"
                                       "    vec3 lightDir = normalize(lightPos);\n"
                                       "    float diff = max(dot(norm, lightDir), 0.0);\n"
                                       "    vec3 diffuse = diff * lightColor;\n"
                                       "\n"
                                       "    // Specular lighting\n"
                                       "    float specularStrength = 0.5;\n"
                                       "    vec3 viewDir = normalize(viewPos - FragPos);\n"
                                       "    vec3 reflectDir = reflect(-lightDir, norm);  \n"
                                       "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
                                       "    vec3 specular = specularStrength * spec * lightColor;  \n"
                                       "\n"
                                       "    // Combining the three components\n"
                                       "    vec3 result = (ambient + diffuse + specular) * ColorsVector;\n"
                                       "    FragColor = vec4(result, 1.0f);\n"
                                       "}";

    glm::mat4 camToWorld = convertToGLMmat4(scene.camera.cam_to_world);

    glm::vec3 cameraPos = glm::vec3(camToWorld[3]);      // Camera position is in the 4th column
    glm::vec3 cameraFront = -glm::vec3(camToWorld[2]);   // Forward vector is negative Z (third column)
    glm::vec3 cameraUp = glm::vec3(camToWorld[1]);       // Up vector is the Y axis (second column)

    float deltaTime = 0.0f;                               // Time between current frame and last frame
    float lastFrame = 0.0f;                               // Time of last frame




    camera = new MyCamera(SCR_WIDTH, SCR_HEIGHT, cameraPos, cameraUp, cameraFront);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW_3_4", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);



    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    std::vector<GLuint> VAOs;
    for (auto &mesh : scene.meshes) {
        unsigned int VAO, VBO_vertex, VBO_color, VBO_normal, EBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &VBO_normal);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(Vector3f), mesh.vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        VAOs.push_back(VAO);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Measure time difference between frames
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera->ProcessKeyboard(window, deltaTime);


        glClearColor(background[0], background[1], background[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        glUseProgram(shaderProgram);

        float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
        float scale = scene.camera.s; // This is the scaling/film size parameter
        float zNear = scene.camera.z_near;
        float zFar = scene.camera.z_far;
        glm::mat4 projection_matrix = glm::mat4(
                1.0f / (aspectRatio * scale), 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f / scale, 0.0f, 0.0f,
                0.0f, 0.0f, -zFar / (zFar - zNear), -1.0f,
                0.0f, 0.0f, -(zFar * zNear) / (zFar - zNear), 0.0f
        );


        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Calculate view matrix once
        glm::mat4 view_matrix = camera->GetViewMatrix();  // Get the view matrix from MyCamera

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);


        //lightcolor 111
        unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3f(lightColorLoc, 1.0,1.0,1.0);

        if (!hw_3_4_extra) {
            //lightpos 111
            unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
            glUniform3f(lightPosLoc, 1.0,1.0,1.0);
        } else {
            //lightpos 000
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Update light position based on time
            float lightX = sin(currentFrame) * 2.0f;
            float lightY = sin(currentFrame) * 2.0f;
            float lightZ = cos(currentFrame) * 2.0f;
            unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
            glUniform3f(lightPosLoc, lightX, lightY, lightZ);
        }

        // Render each mesh
        for (int i = 0; i < scene.meshes.size(); i++) {
            glm::mat4 model_matrix = convertToGLMmat4(scene.meshes[i].model_matrix);
            unsigned int model_matrixLoc = glGetUniformLocation(shaderProgram, "model_matrix");
            glUniformMatrix4fv(model_matrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, scene.meshes[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    for (int i = 0; i < VAOs.size(); i++) {
        glDeleteVertexArrays(1, &VAOs[i]);
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    //free camera
    delete camera;
}

/**
 * @brief HW 3.5: Wrapper function for HW 3.3
 * @param params json file
 */
void hw_3_5(const std::vector<std::string> &params) {
    hw_3_3(params);
}


