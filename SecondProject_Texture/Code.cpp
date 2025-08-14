#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <custom/program.h>
#include <iostream>
#include <vector>

// Translation includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 2D Array setup
using glm2DArray = std::vector<glm::vec3>;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void doAllTransformations(glm::mat4& translationMatrix, glm2DArray translationVals, float rotationAngles[], glm2DArray rotationAxes, glm2DArray scaleValues);
void updateRotationAngle(int whichRotationAsIndex, float newValue, float rotationAngles[]);

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

     // Cube stuff
     float cubeVertices[] = {
          // Viewport coords   // Color            // Texture coords
          // Front
          0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // front top right     0
          0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // front bottom right  1
         -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // front bottom left   2
         -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // front top left      3
         // Back
          0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // back top right      4
          0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // back bottom right   5
         -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // back bottom left    6
         -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // back top left       7
     };

     unsigned int cubeIndices[] = {
          // Front
          2, 1, 3,
          3, 1, 0,
          // Back
          6, 5, 7,
          7, 5, 4,
          // Right
          0, 1, 5,
          0, 5, 4,
          // Left
          7, 6, 2,
          7, 2, 3,
          // Top
          7, 3, 0,
          7, 0, 4,
          // Bottom
          6, 2, 1,
          6, 1, 5
     };
     GLsizei numOfCubeIndices = sizeof(cubeIndices) / sizeof(unsigned int);

     glm::vec3 cubePositions[] = {
          glm::vec3(0.0f,  0.0f,  0.0f), // Original
          glm::vec3(2.0f,  5.0f, -15.0f),
          glm::vec3(-1.5f, -2.2f, -2.5f),
          glm::vec3(-3.8f, -2.0f, -12.3f),
          glm::vec3(2.4f, -0.4f, -3.5f),
          glm::vec3(-1.7f,  3.0f, -7.5f),
          glm::vec3(1.3f, -2.0f, -2.5f),
          glm::vec3(1.5f,  2.0f, -2.5f),
          glm::vec3(1.5f,  0.2f, -1.5f),
          glm::vec3(-1.3f,  1.0f, -1.5f)
     };


     // Buffers and VAO
     unsigned int VBOcube, VAOcube, EBOcube;
     glGenVertexArrays(1, &VAOcube);
     glGenBuffers(1, &VBOcube);
     glGenBuffers(1, &EBOcube);

     glBindVertexArray(VAOcube);
     glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
     glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOcube);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

     // Vertex attribs
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(float)));
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);

     // Unbind stuff
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


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

     // 3D matrices
     unsigned int modelLoc = glGetUniformLocation(recProgram.ID, "model");
     unsigned int viewLoc = glGetUniformLocation(recProgram.ID, "view");
     unsigned int projectionLoc = glGetUniformLocation(recProgram.ID, "projection");

          // Transformation
     unsigned int transformLoc = glGetUniformLocation(recProgram.ID, "transform");
     

          // Arrays for holding the various transformation information needed by glm

     glm2DArray translationVectors{
          { 0.25,  0.25, 0.25},
          {-0.50, -0.50, 0.0}
     };
     float rotationAngles[] = { // The number of entries here needs to match the number entires in rotationAxes
          45.0,
          90.0
     };
     glm2DArray rotationAxes = {
          {0.0, 0.0, 1.0},
          {1.0, 0.0, 0.0}
     };
     glm2DArray scaleVectors = {
          {0.5, 0.5, 0.5}
     };

     glEnable(GL_DEPTH_TEST);

     // Render loop
     while (!glfwWindowShouldClose(window)) {
          // Input
          processInput(window);

          // Render/draw
          glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

               // Shader texture activations
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, texture);
          glActiveTexture(GL_TEXTURE1);
          glBindTexture(GL_TEXTURE_2D, texture2);
          
          recProgram.use(); // Even though we only have one program we should use it here for practice; if we wanted to use multiple we would need to
          // Transformation
          glm::mat4 trans = glm::mat4(1.0f);
          float dynamicInRadians = (float)glfwGetTime() * (180/ 3.1415);
          updateRotationAngle(0, dynamicInRadians, rotationAngles);
          updateRotationAngle(1, dynamicInRadians, rotationAngles);
          doAllTransformations(trans, translationVectors, rotationAngles, rotationAxes, scaleVectors);
          
          // 3D stuff
          glm::mat4 model = glm::mat4(1.0f); // Worldspace
          glm::mat4 view = glm::mat4(1.0f); // Camera
          glm::mat4 projection = glm::mat4(1.0f); // Clip/perspective
          
          model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
          view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
          projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

          // Send uniforms information
          glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
          glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
               // We set the projection matrix each frame here, but in practice it rarely changes and so its better to set it once outside the render loop
          glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
          
          glBindVertexArray(VAOcube);
          glDrawElements(GL_TRIANGLES, numOfCubeIndices, GL_UNSIGNED_INT, 0);
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

// Does all transformation/translation changes to the object via glm functions
void doAllTransformations(glm::mat4 &translationMatrix, glm2DArray translationVals, float rotationAngles[], glm2DArray rotationAxes, glm2DArray scaleValues) {
     // Translations
     for (int i = 0; i < translationVals.size(); i++) {
          translationMatrix = glm::translate(translationMatrix, translationVals.at(i));
     }

     // Rotations
          // Make sure the number of angles matches the number of rotation axes
     int anglesLength = sizeof(rotationAngles) / sizeof (float);
     int axesLength = rotationAxes.size();
     if (anglesLength == axesLength) {
          for (int i = 0; i < rotationAxes.size(); i++) {
               translationMatrix = glm::rotate(translationMatrix, glm::radians(rotationAngles[i]), rotationAxes.at(i));
          }
     }
     else {
          std::cout << "Numer of rotation angles doesn't match number of rotation axes: No rotation occruing" << std::endl;
     }
     
     // Scalings
     for (int i = 0; i < scaleValues.size(); i++) {
          translationMatrix = glm::scale(translationMatrix, scaleValues.at(i));
     }
}

void updateRotationAngle(int whichRotationAsIndex, float newValue , float rotationAngles[]) {
     if (whichRotationAsIndex < 0) {
          return;
     }

     int numOfRotations = sizeof(rotationAngles) / sizeof(float);
     if (whichRotationAsIndex < numOfRotations) {
          rotationAngles[whichRotationAsIndex] = newValue;
     }
     else {
          std::cout << "Given index is beyond size of rotationAngles: No change made" << std::endl;
     }
}