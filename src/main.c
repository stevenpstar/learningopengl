#include "../dep/glad/glad.h"
#include "../dep/GLFW/glfw3.h"
#include <stdio.h>
#include <stdbool.h>
// engine files
#include "engine/primitives.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/camera.h"
// game files
#include "game/Player.h"

// globals
double mouseX;
double mouseY;
Camera *activeCamera;
Sprite *playerSprite;
Player *activePlayer;
Animation playerAnim = {
  .state = IDLE_DOWN,
  .startFrame = 0,
  .endFrame = 1,
};
bool firstCamMove = false;
bool updateProjection = true;
bool perspective = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
vec3 lightPos = {0.0f, -2.0f, 0.0f};
bool debugPrint44 = true;

bool idle = true;
bool forwardDown = false;
bool backDown = false;
bool rightDown = false;
bool leftDown = false;
float tc = 0.0f;

vec3 up = {0.0f, 1.0f, 0.0f};

unsigned int VBO, lVBO, VAO, EBO, lightingVAO, shader, lightingShader, texture;

void debug_PRINTMAT4(mat4x4 printme) {
  printf("Mat4x4: %f, %f, %f, %f\n", printme[0][0], printme[0][1], printme[0][2], printme[0][3]);
  printf("Mat4x4: %f, %f, %f, %f\n", printme[1][0], printme[1][1], printme[1][2], printme[1][3]);
  printf("Mat4x4: %f, %f, %f, %f\n", printme[2][0], printme[2][1], printme[2][2], printme[2][3]);
  printf("Mat4x4: %f, %f, %f, %f\n", printme[3][0], printme[3][1], printme[3][2], printme[3][3]);
  printf("----");
}

void resizeWindow(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  // TESTING THE CHANGE OF FOV
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
//    setFOV(activeCamera->fov + 0.1f, activeCamera);
//    if (!updateProjection) {
//      updateProjection = true;
//      perspective = !perspective;
//      printf("Persp %d\n", perspective);
//    }
    GLint polygonMode[2];
    glad_glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    if (polygonMode[0] == GL_LINE)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else 
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  }
  if (key == GLFW_KEY_N && action == GLFW_PRESS) {
    playerSprite->currentFrame++;
    SetFrame(playerSprite, playerSprite->currentFrame, VBO, false);
    printf("playerSprite currentFrame %d\n", playerSprite->currentFrame);
  }
  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    forwardDown = true;
  } else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
    forwardDown = false;
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    backDown = true;
  } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
    backDown = false;
  }
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    leftDown = true;
  } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
    leftDown = false;
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    rightDown = true;
  } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
    rightDown = false;
  }
}

void processCamMovement() {
  vec3 camFront = {0.0f, 0.0f, -1.0f};
  vec3 nFront;
  if (forwardDown) {
    vec3_scale(nFront, activeCamera->direction, 0.025f);
    vec3_add(activeCamera->position, activeCamera->position, nFront);
  }
  if (backDown) {
    vec3_scale(nFront, activeCamera->direction, -0.025f);
    vec3_add(activeCamera->position, activeCamera->position, nFront);
  }
  if (leftDown) {
    vec3 cross;
    vec3_mul_cross(cross, activeCamera->direction, up);
    vec3_norm(cross, cross);
    vec3_scale(cross, cross, -0.025f);
    vec3_add(activeCamera->position, activeCamera->position, cross);
  }
  if (rightDown) {
    vec3 cross;
    vec3_mul_cross(cross, activeCamera->direction, up);
    vec3_norm(cross, cross);
    vec3_scale(cross, cross, 0.025f);
    vec3_add(activeCamera->position, activeCamera->position, cross);
  }
  if (!forwardDown && !backDown && !leftDown && !rightDown) {
    idle = true;
  } else {
    idle = false;
  }
}

void processPlayerMovement(float deltaTime) {
  float playerSpeed = 1.f;
  if (forwardDown) {
    if (playerAnim.state != RUN_UP) {
      playerAnim.state = RUN_UP;
      playerAnim.startFrame = 20;
      playerAnim.endFrame = 23;
    }
    activePlayer->y += playerSpeed * deltaTime;
  }
  if (backDown) {
    if (playerAnim.state != RUN_DOWN) {
      playerAnim.state = RUN_DOWN;
      playerAnim.startFrame = 12;
      playerAnim.endFrame = 15;
    }
    activePlayer->y -= playerSpeed * deltaTime;
  }
  if (leftDown) {
    if (playerAnim.state != RUN_LEFT) {
      playerAnim.state = RUN_LEFT;
      playerAnim.startFrame = 16;
      playerAnim.endFrame = 19;
    }

    activePlayer->x -= playerSpeed * deltaTime;
  }
  if (rightDown) {
  if (playerAnim.state != RUN_RIGHT) {
      playerAnim.state = RUN_RIGHT;
      playerAnim.startFrame = 16;
      playerAnim.endFrame = 19;
    }
    activePlayer->x += playerSpeed * deltaTime;
  }
  if (!forwardDown && !backDown && !leftDown && !rightDown) {
    if (playerAnim.state == RUN_UP) {
      playerAnim.state = IDLE_UP;
      playerAnim.startFrame = 8;
      playerAnim.endFrame = 9;
    }
    else if (playerAnim.state == RUN_RIGHT) {
      playerAnim.state = IDLE_RIGHT;
      playerAnim.startFrame = 4;
      playerAnim.endFrame = 5;
    }
    else if (playerAnim.state == RUN_LEFT) {
      playerAnim.state = IDLE_LEFT;
      playerAnim.startFrame = 4;
      playerAnim.endFrame = 5;
    }
    else if (playerAnim.state == RUN_DOWN) {
      playerAnim.state = IDLE_DOWN;
      playerAnim.startFrame = 0;
      playerAnim.endFrame = 1;
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

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  vec3 pos = {0.0f, 0.0f, -5.0f};
  vec3 target = {0.0f, 0.0f, 0.0f};
  Camera cam = createCamera(pos, target, 2.5f);
  activeCamera = &cam;
  shader = createShader("./src/shaders/vertex.vert", 
      "./src/shaders/fragment.frag");
  texture = loadTexture("res/placeholder.png");

  Sprite player = createAnimatedSprite(VBO, EBO, 0.0f, 0.0f, -12.0f, "res/Prototype_Character.png",
      32, 32, 128, 384); 
  playerSprite = &player;
  Player playerObj = {
    .x = 0.0f,
    .y = 0.0f,
    .z = -12.0f,
    .sprite = &player,
    .state = 0,
    .framerate = 8,
    .frameTimer = 0.0f,
  };
  activePlayer = &playerObj;
  //P_CUBE cube = createCube(VBO);
  GLint bufsize = 0;
  float data[24];
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufsize);
  printf("bufsize: %d\n", bufsize);
  glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufsize, data);
  for (int i=0;i<24;i++) {
    printf("bufsize: %f\n", data[i]);
  }
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "tex1"), 0);

  // load texture into shader

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, resizeWindow);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouseMove);
  glfwSetKeyCallback(window, keyCallback);
  // Set projection before game loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (updateProjection) {
      glUseProgram(shader);
      setProjection(shader, "proj", activeCamera, perspective);
      updateProjection = false;
    }
//    processCamMovement();
    processPlayerMovement(deltaTime);
    Animate(&playerObj, playerAnim, true, deltaTime, VBO);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, player.texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, player.texture);

    // camera
    vec3 lookAhead;
    vec3_add(lookAhead, cam.position, cam.direction);
    mat4x4 view;
    mat4x4_identity(view);
    mat4x4_translate_in_place(view, cam.position[0], cam.position[1], 
        cam.position[2]);
    mat4x4_look_at(view, cam.position, lookAhead, up);

    unsigned int viewLoc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (GLfloat *)view);

    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate_in_place(model, playerObj.x, playerObj.y, playerObj.z);
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)model);

    glUseProgram(shader);
    glBindVertexArray(VAO);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  //  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}
