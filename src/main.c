#include "../dep/glad/glad.h"
#include "../dep/GLFW/glfw3.h"
#include <stdio.h>
#include <stdbool.h>
#include "engine/primitives.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/camera.h"

// globals
double mouseX;
double mouseY;
Camera *activeCamera;
bool firstCamMove = false;
bool updateProjection = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
void resizeWindow(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  // TESTING THE CHANGE OF FOV
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    setFOV(activeCamera->fov + 0.1f, activeCamera);
    if (!updateProjection) {
      updateProjection = true;
    }
  }
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
  if (firstCamMove) {
    mouseX = xpos;
    mouseY = ypos;
    firstCamMove = false;
  }
  // This is for camera mouse look (fps, spectator camera etc.)
  // Will need to be gated behind some flag at some point
  float xoffset = xpos - mouseX;
  float yoffset = mouseY - ypos;  
  mouseX = xpos;
  mouseY = ypos;

  mouseLook(xoffset, yoffset, activeCamera, deltaTime);
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Base", NULL, NULL);
  if (window == NULL) {
    printf("GLFW Window creation failed\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialise glad");
    glfwTerminate();
    return -1;
  }

  unsigned int VBO, VAO, shader, texture;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  P_CUBE cube = createCube(VBO);
  P_CUBE cube2 = createCube(VBO);
  cube2.posY = 1.5f;
  P_CUBE cubes[2] = {cube, cube2};
  vec3 pos = {0.0f, 0.0f, -3.0f};
  vec3 target = {0.0f, 0.0f, 0.0f};
  Camera cam = createCamera(pos, target, 2.5f);
  activeCamera = &cam;
  texture = loadTexture("res/placeholder.png");

  shader = createShader("./src/shaders/vertex.vert", 
      "./src/shaders/fragment.frag");

  // load texture into shader
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "tex1"), 0);
  glUniform1i(glGetUniformLocation(shader, "tex2"), 1);

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, resizeWindow);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouseMove);
  glfwSetKeyCallback(window, keyCallback);
  // Set projection before game loop
  while (!glfwWindowShouldClose(window)) {
    if (updateProjection) {
      setProjection(shader, "proj", activeCamera);
      updateProjection = false;
    }
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // This isn't right
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // this should be per model

    // camera
    vec3 lookAhead;
    vec3_add(lookAhead, cam.position, cam.direction);
    mat4x4 view;
    mat4x4_identity(view);
    mat4x4_translate_in_place(view, cam.position[0], cam.position[1], 
        cam.position[2]);
    vec3 up = {0.0f, 1.0f, 0.0f};
    mat4x4_look_at(view, cam.position, lookAhead, up);

    unsigned int viewLoc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (GLfloat *)view);

    for (int i=0;i<2;i++) {
      mat4x4 model;
      mat4x4_identity(model);
      mat4x4_translate_in_place(model, cubes[i].posX, cubes[i].posY, cubes[i].posZ);
      mat4x4_rotate(model, model, 0.0f, 1.0f, 0.0f, (float)glfwGetTime() * 0.9995);
      unsigned int modelLoc = glGetUniformLocation(shader, "model");
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
