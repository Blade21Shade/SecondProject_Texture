#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <custom/program.h>
#include <stb/stb_image.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
     // GLFW setup
     glfwInit();
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

     // Load glad
     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
          std::cout << "Failed to initialize GLAD" << std::endl;
          return -1;
     }

     // Initial window creation and viewport setup
     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window Title", NULL, NULL);
     if (!window) {
          std::cout << "Failed to create GLFW window" << std::endl;
          glfwTerminate();
          return -1;
     }
     glfwMakeContextCurrent(window);
     
     glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
     glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);


     // Setup shaders
     Program recProgram("vertexShader.vert", "fragmentShader.vert");

     // Setup shape data
     float recVertices[] = {
          // Viewport coords   // Color            // Texture coords
          0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
          0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
         -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
         -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
     };

     unsigned int recIndices[] = {
          2, 1, 3, // Triangle 1
          3, 1, 0 // Triangle 2
     };

     // Buffers and VAO
     unsigned int VBO, VAO, EBO;
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
     glGenVertexArrays(1, &VAO);

     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(recVertices), recVertices, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(recIndices), recIndices, GL_STATIC_DRAW);

          // Vertex attribs
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(float)));
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);

     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

     // Render loop
     while (!glfwWindowShouldClose(window)) {
          // Input
          processInput(window);

          // Render/draw
          glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT);

          recProgram.use();
          glBindVertexArray(VAO);
          glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);


          // Poll events
          glfwPollEvents();

          // Swap buffers
          glfwSwapBuffers(window);
     }

     // Cleanup and return
     glDeleteVertexArrays(1, &VAO);
     glDeleteBuffers(1, &EBO);
     glDeleteBuffers(1, &VBO);
     recProgram.deleteProgram();

     glfwTerminate();
     return 1;
}

// Utility functions

// Resize the viewport when the user expands the window
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
     glViewport(0, 0, width, height);
}

// General user input (keyboard only for now)
void processInput(GLFWwindow* window) {
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
          glfwSetWindowShouldClose(window, true);
     }
}