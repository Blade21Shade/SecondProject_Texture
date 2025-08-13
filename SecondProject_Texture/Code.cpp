#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <custom/program.h>
#include <iostream>

// Translation includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

     // Initial window creation and viewport setup
     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window Title", NULL, NULL);
     if (!window) {
          std::cout << "Failed to create GLFW window" << std::endl;
          glfwTerminate();
          return -1;
     }
     glfwMakeContextCurrent(window);

     // Load glad
     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
          std::cout << "Failed to initialize GLAD" << std::endl;
          return -1;
     }
     
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
          2, 1, 3, // Bottom triangle
          3, 1, 0  // Top triangle
     };
     GLsizei numOfRecIndices = sizeof(recIndices) / sizeof(unsigned int); // So the draw call uses a calculated value isntead of a set one, less error prone

     // Buffers and VAO
     unsigned int VBO, VAO, EBO;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);

     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(recVertices), recVertices, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(recIndices), recIndices, GL_STATIC_DRAW);

          // Vertex attribs
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(float)));
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);

     glBindBuffer(GL_ARRAY_BUFFER, 0);
     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); !!! This is wrong, you aren't supposed to unbind the element buffer before unbinding a VAO
     glBindVertexArray(0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

     // Setup image and texture 
          // Texture setup
     unsigned int texture;
     glGenTextures(1, &texture);
     glBindTexture(GL_TEXTURE_2D, texture);
               // Params
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          // Image
     int width, height, nrChannels;
     unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
               // Attempt to load image into texture
     if (data) {
          // Load image into texture
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
          glGenerateMipmap(GL_TEXTURE_2D);
     }
     else {
          std::cout << "Failed to load texture" << std::endl;
     }
     stbi_image_free(data); // Free the image data once we've finished loading it    

          // Second image
     unsigned int texture2;
     glGenTextures(1, &texture2);
     glBindTexture(GL_TEXTURE_2D, texture2); // This should kick "texture" off
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // The new texture doesn't have these bound yet, we need to do so again
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     stbi_set_flip_vertically_on_load(true);
     data = stbi_load("awesomeSmile.png", &width, &height, &nrChannels, 0); // The setup was done before, so just bopy and paste
     if (data) {
          // Load image into texture
          // .png has A value so we need to specify that when loading, note we still store it as GL_RGB
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
          glGenerateMipmap(GL_TEXTURE_2D);
     }
     else {
          std::cout << "Failed to load texture" << std::endl;
     }
     stbi_image_free(data); // Free the image data once we've finished loading it   

     // The uniforms only need to be set once, so they can be done outside the loop
     // You still need to use the program before setting them
     recProgram.use();
     // We need to send the location values
     glUniform1i(glGetUniformLocation(recProgram.ID, "ourTexture"), 0); // set it manually
     recProgram.setInt("ourTexture2", 1); // or with shader class

          // Translation
     //glm::mat4 trans = glm::mat4(1.0f);
     //trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)); // Rotate 90 degrees about the z-axis
     //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
               // Sending it to the program
     unsigned int transformLoc = glGetUniformLocation(recProgram.ID, "transform");
     /*glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));*/

     // Render loop
     while (!glfwWindowShouldClose(window)) {
          // Input
          processInput(window);

          // Render/draw
          glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT);

               // Shader activations
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, texture);
          glActiveTexture(GL_TEXTURE1);
          glBindTexture(GL_TEXTURE_2D, texture2);
          
          recProgram.use(); // Even though we only have one program we should use it here for practice; if we wanted to use multiple we would need to
          // Translation
          glm::mat4 trans = glm::mat4(1.0f);
          trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0)); // Rotate 90 degrees about the z-axis
               // Sending it to the program
          glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
          
          
          glBindVertexArray(VAO);
          glDrawElements(GL_TRIANGLES, numOfRecIndices, GL_UNSIGNED_INT, 0);
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
     return 0;
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