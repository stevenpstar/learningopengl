#include "../dep/glad/glad.h"
#include "../dep/GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
// engine files
#include "engine/primitives.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/camera.h"
#include "engine/collision.h"
#include "engine/lighting.h"
// game files
#include "game/Player.h"
#include "game/tile.h"

// globals
double mouseX;
double mouseY;
int windowWidth = 800;
int windowHeight = 600;
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
bool fpsMode = false;

bool idle = true;
bool forwardDown = false;
bool backDown = false;
bool rightDown = false;
bool leftDown = false;
float tc = 0.0f;

Bounds boundsA = {
  .x = 0.0f,
  .y = 0.0f,
  .width = 0.0f,
  .height = 0.0f
};

Bounds boundsB = {
  .x = 0.0f,
  .y = 0.0f,
  .width = 0.0f,
  .height = 0.0f
};

vec3 up = {0.0f, 1.0f, 0.0f};

unsigned int VBO, CubeVBO, worldVBO, lVAO, lVBO, VAO, EBO, lightingVAO, shader, lightingShader, texture, bsq;
int tiles[1024] = {0};

void debug_PRINTMAT4(mat4x4 printme) {
  printf("Mat4x4: %f, %f, %f, %f\n", printme[0][0], printme[0][1], printme[0][2], printme[0][3]);
  printf("Mat4x4: %f, %f, %f, %f\n", printme[1][0], printme[1][1], printme[1][2], printme[1][3]);
  printf("Mat4x4: %f, %f, %f, %f\n", printme[2][0], printme[2][1], printme[2][2], printme[2][3]);
  printf("Mat4x4: %f, %f, %f, %f\n", printme[3][0], printme[3][1], printme[3][2], printme[3][3]);
  printf("----");
}

void resizeWindow(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  windowWidth = width;
  windowHeight = height;
  glUseProgram(shader);
  setProjection(shader, "proj", activeCamera, perspective, windowWidth, windowHeight);
  glUseProgram(lightingShader);
  setProjection(lightingShader, "proj", activeCamera, perspective, windowWidth, windowHeight);
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
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    fpsMode = !fpsMode;
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

void processPlayerMovement(float deltaTime) {
  if (fpsMode) { return; }
  float playerSpeed = 2.f;
  float diagonalMod = 0.70710678118;

  int mVert = 0;
  int mHoriz = 0;

  if (forwardDown && rightDown || forwardDown && leftDown ||
      backDown && rightDown || backDown && leftDown) {
    playerSpeed *= diagonalMod;
  }
  if (forwardDown) {
    if (playerAnim.state != RUN_UP) {
      playerAnim.state = RUN_UP;
      playerAnim.startFrame = 20;
      playerAnim.endFrame = 23;
    }
    mVert = -1;
  //  activePlayer->y += playerSpeed * deltaTime;
  //  activeCamera->position[1] = activePlayer->y;
  }
  if (backDown) {
    if (playerAnim.state != RUN_DOWN) {
      playerAnim.state = RUN_DOWN;
      playerAnim.startFrame = 12;
      playerAnim.endFrame = 15;
    }
    mVert = 1;
//    activePlayer->y -= playerSpeed * deltaTime;
//    activeCamera->position[1] = activePlayer->y;
  }
  if (leftDown) {
    if (playerAnim.state != RUN_LEFT) {
      playerAnim.state = RUN_LEFT;
      playerAnim.startFrame = 16;
      playerAnim.endFrame = 19;
    }
    mHoriz = -1;
//    activePlayer->x -= playerSpeed * deltaTime;
//    activeCamera->position[0] = activePlayer->x;
  }
  if (rightDown) {
  if (playerAnim.state != RUN_RIGHT) {
      playerAnim.state = RUN_RIGHT;
      playerAnim.startFrame = 16;
      playerAnim.endFrame = 19;
    }
    mHoriz = 1;
//    activePlayer->x += playerSpeed * deltaTime;
//    activeCamera->position[0] = activePlayer->x;
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
  vec2 nextPosition = {0};
  nextPosition[0] = activePlayer->x + mHoriz * playerSpeed * deltaTime;
  nextPosition[1] = activePlayer->y + mVert * playerSpeed * deltaTime;
  boundsA.width = 0.5f;
  boundsA.height = 0.5f;
  boundsA.x = nextPosition[0];
  boundsA.y = nextPosition[1];
  int nextTile[2] = {0};
  getTileFromPosition(nextPosition[0], nextPosition[1], nextTile);
  int tileIndex = getIndexFromTile(nextTile[0], nextTile[1], 32);
  bool checkCollide = tiles[tileIndex] == 34;

  boundsB.width = 0.5f;
  boundsB.height = 0.5f;
  boundsB.x = nextTile[0] * 0.5f;
  boundsB.y = nextTile[1] * 0.5f;

  if (!IsColliding(&boundsA, &boundsB) || !checkCollide) {
    activePlayer->x += mHoriz * playerSpeed * deltaTime;
    activePlayer->z += mVert * playerSpeed * deltaTime;
    if (!fpsMode) {
      activeCamera->position[0] = -activePlayer->x;
      activeCamera->position[1] = activePlayer->z;
    }
  }
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
  if (!fpsMode) { return; }
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
  srand(time(NULL));
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
  glGenVertexArrays(1, &lVAO);
  glGenVertexArrays(1, &lightingVAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &CubeVBO);
  glGenBuffers(1, &worldVBO);
  glGenBuffers(1, &lVBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  vec3 pos = {0.0f, 0.0f, -7.0f};
  vec3 fpsPos = {0.0f, 0.5f, 0.0f};
  vec3 target = {0.0f, 0.0f, 0.0f};
  Camera cam = createCamera(pos, target, 2.5f);
  Camera FPScam = createCamera(fpsPos, target, 2.5f);
  activeCamera = &cam;
  shader = createShader("./src/shaders/vertex.vert", 
      "./src/shaders/fragment.frag");
  lightingShader = createShader("./src/shaders/vertex.vert", 
      "./src/shaders/lighting.frag");

  texture = loadTextureRGB("res/wallhue.png");
  bsq = loadTextureRGB("res/blacksq.png");

  Sprite player = createAnimatedSprite(VBO, EBO, 0.0f, 0.0f, -12.0f, "res/Prototype_Character.png",
      32, 32, 128, 384); 
  playerSprite = &player;
  Player playerObj = {
    .x = 0.0f,
    .y = 0.25f,
    .z = 0.0f,
    .sprite = &player,
    .state = 0,
    .framerate = 8,
    .frameTimer = 0.0f,
  };
  glBindVertexArray(lightingVAO);

  unsigned int leveltex = loadTextureRGB("res/testlevelp.png");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, leveltex);
  GLubyte pixels[32*32*3] = {0};
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &pixels);
  glGetError();

  unsigned int worldTex = createWorld(tiles, "res/floortiles.png", pixels);
  activePlayer = &playerObj;
  P_CUBE cube = createCube(CubeVBO);
  glBindVertexArray(lVAO);
//  P_CUBE_LIGHT lcube = createCubeLight(lVBO, 0.0f, 0.5f, 1.0f);
  P_CUBE_LIGHT lightCubes[4] = {
    createCubeLight(lVBO, 0.0f, 0.5f, 1.0f),
    createCubeLight(lVBO, 2.0f, 0.5f, 1.0f),
    createCubeLight(lVBO, 9.0f, 0.5f, 9.0f),
    createCubeLight(lVBO, 24.0f, 0.5f, 24.0f),
  };
  glBindVertexArray(lightingVAO);
  GLint bufsize = 0;
  float data[24];
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufsize);
  printf("bufsize: %d\n", bufsize);
  glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufsize, data);
  for (int i=0;i<24;i++) {
    printf("bufsize: %f\n", data[i]);
  }
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "material.diffuse"), 0);

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
  float planeData[48] = {
    -0.5f, -0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0 - 0.083333f,//0.0f, // bottom left
     0.5f, -0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.25f, 1.0 - 0.083333f, // bottom right
     0.5f,  0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.25f, 1.f, // top right
     0.5f,  0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.25f, 1.f, // top right duplicate (ignore and/or change to be same not sure)
    -0.5f,  0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
    -0.5f, -0.5f, -0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0 - 0.083333f, // bottom left duplicate     
  };

  while (!glfwWindowShouldClose(window)) {
    if (fpsMode) {
      activeCamera = &FPScam;
    } else {
      activeCamera = &cam;
    }
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (updateProjection) {
      glUseProgram(shader);
      setProjection(shader, "proj", activeCamera, perspective, windowWidth, windowHeight);
      glUseProgram(lightingShader);
      setProjection(lightingShader, "proj", activeCamera, perspective, windowWidth, windowHeight);
      updateProjection = false;
    }
    if (fpsMode) {
      processCameraMovement(activeCamera, &idle, forwardDown, 
          backDown, leftDown, rightDown, fpsMode, up);
    }
    processPlayerMovement(deltaTime);
    glBindVertexArray(VAO);
    glUseProgram(shader);
//    glUniform3f(glGetUniformLocation(shader, "light.position"), lcube.posX, lcube.posY, lcube.posZ);
    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, activeCamera->position);
        // dirLight properties
    glUniform3f(glGetUniformLocation(shader, "dirLight.direction"), -0.2f, -1.0f, -0.3f); 
    glUniform3f(glGetUniformLocation(shader, "dirLight.ambient"), 0.2f, 0.2f, 0.2f); 
    glUniform3f(glGetUniformLocation(shader,"dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(shader, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
      // point lights
    for (int i = 0; i < 4; i++) {
      vec3 lPos = { lightCubes[i].posX, lightCubes[i].posY, lightCubes[i].posZ };
      vec3 diff = { 0.8f, 0.8f, 0.8f };
      setPointLight(i, lPos, diff, shader);
    }
    // material properties
    glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(shader, "material.shininess"), 64.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, player.texture);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    // camera
    vec3 lookAhead;
    vec3_add(lookAhead, activeCamera->position, activeCamera->direction);
    mat4x4 view;
    mat4x4_identity(view);
    mat4x4_translate_in_place(view, activeCamera->position[0], activeCamera->position[1], 
        activeCamera->position[2]);
    if (fpsMode) {
      mat4x4_look_at(view, activeCamera->position, lookAhead, up);
    } else {
      mat4x4_rotate(view, view, 1.0f, 0.0f, 0.0f, degToRad(90.0f) );
    }

    unsigned int viewLoc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (GLfloat *)view);
    glBindVertexArray(lightingVAO);
    glBindBuffer(GL_VERTEX_ARRAY, VBO);
    for (int i=0;i<1024;i++) {
      int row = floor((float)i / 32);
      int col = i - (32 * row);
     // printf("row %d, col: %d\n", row, col);
      setTileData(tiles[i], 16, 112, 112, pixels, planeData, worldVBO);
      mat4x4 tile;
      mat4x4_identity(tile);
      if (tiles[i] != 34) {
        mat4x4_translate_in_place(tile, col * 0.5f, 0.0f, row * 0.5f);
        mat4x4_rotate(tile, tile, 1.0f, 0.0f, 0.0f, degToRad(90.0f));
        mat4x4_scale_aniso(tile, tile, 0.5f, 0.5f, 0.5f);
        unsigned int modelLoc = glGetUniformLocation(shader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)tile);
//        glUniform1i(glGetUniformLocation(shader, "material.diffuse"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, worldTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
      } else {
        resetTileTexCoords(planeData, worldVBO);
        // mega scuffed
        for (int c=0;c<4;c++) {
          mat4x4_identity(tile);
          if (c == 0) {
            mat4x4_translate_in_place(tile, (col * 0.5f), 0.25f, (row * 0.5f) - 0.25f);
          }
          if (c == 1) {
            mat4x4_translate_in_place(tile, (col * 0.5f), 0.25f, (row * 0.5f) + 0.25f);
          }
          if (c == 2) {
            mat4x4_translate_in_place(tile, (col * 0.5f) - 0.25f, 0.25f, (row * 0.5f));
            mat4x4_rotate(tile, tile, 0.0f, 1.0f, 0.0f, degToRad(90.0f));
          }
          if (c == 3) {
            mat4x4_translate_in_place(tile, (col * 0.5f) + 0.25f, 0.25f, (row * 0.5f));
            mat4x4_rotate(tile, tile, 0.0f, 1.0f, 0.0f, degToRad(90.0f));
          }
          mat4x4_scale_aniso(tile, tile, 0.5f, 0.5f, 0.5f);
          unsigned int modelLoc = glGetUniformLocation(shader, "model");
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)tile);

  //        updateCubeVBO(CubeVBO, &cube);
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, texture);
          glBindVertexArray(lightingVAO);
          glDrawArrays(GL_TRIANGLES, 0, 6);

          // scuffed double height walls?
          mat4x4_translate_in_place(tile, 0.0f, 1.f, 0.0f);
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)tile);
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, texture);
          glBindVertexArray(lightingVAO);
          glDrawArrays(GL_TRIANGLES, 0, 6);

        }
        // mega scuffed roof for walls
        mat4x4_identity(tile);
        mat4x4_translate_in_place(tile, col * 0.5f, 1.f, row * 0.5f);
        mat4x4_rotate(tile, tile, 1.0f, 0.0f, 0.0f, degToRad(90.0f));
        mat4x4_scale_aniso(tile, tile, 0.5f, 0.5f, 0.5f);
        unsigned int modelLoc = glGetUniformLocation(shader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)tile);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bsq);
        glBindVertexArray(lightingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

     }
    }
    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate_in_place(model, playerObj.x, playerObj.y, playerObj.z);
    mat4x4_rotate(model, model, 1.0f, 0.0f, 0.0f, degToRad(-90.0f));
    if (fpsMode) {
      mat4x4_rotate(model, model, -1.0f, 0.0f, 0.0f, degToRad(-90.0f));
    }
    unsigned int modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat *)model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, player.texture);
   // Rendering Player Sprite
    glBindVertexArray(VAO);

    Animate(&playerObj, playerAnim, true, deltaTime, VBO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(lVAO);
    glUseProgram(lightingShader);
    for (int i=0;i<4;i++) {
      mat4x4 light;
      mat4x4_identity(light);
      mat4x4_translate_in_place(light, lightCubes[i].posX, lightCubes[i].posY, lightCubes[i].posZ);
      mat4x4_scale_aniso(light, light, 0.2f, 0.2f, 0.2f);
      unsigned int lightLoc = glGetUniformLocation(lightingShader, "model");
      glUniformMatrix4fv(lightLoc, 1, GL_FALSE, (GLfloat *)light);
      unsigned int lightviewLoc = glGetUniformLocation(lightingShader, "view");
      glUniformMatrix4fv(lightviewLoc, 1, GL_FALSE, (GLfloat *)view);

      setCubeLight(lVBO, lightCubes[i].vertices);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &lVBO);
  glDeleteBuffers(1, &worldVBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}
