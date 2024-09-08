#include "../../dep/glad/glad.h"
#include "../../dep/GLFW/glfw3.h"
#include "../../dep/linmath.h"
//engine
#include "../engine/camera.h"
#include "../engine/shader.h"
#include "../engine/texture.h"
//game
#include "input.h"

unsigned int VBO;
unsigned int VAO;
unsigned int shader, lightingShader;
unsigned int tilesTexture, wallTexture;
Inputs inputs;
int tiles[1024] = {0};

// Cameras
Camera cam;
Camera fpsCam;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {}
void InitGame() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  // setup cameras
  vec3 target = {0.0f, 0.0f, 0.0f};
  vec3 pos = {0.0f, 0.0f, -7.0f};
  vec3 fpsPos = {0.0f, 0.4f, 0.0f};
  cam = createCamera(pos, target, 2.5f);
  fpsCam = createCamera(fpsPos, target, 2.5f);

  shader = createShader("./src/shaders/vertex.vert", 
      "./src/shaders/fragment.frag");
  lightingShader = createShader("./src/shaders/vertex.vert", 
      "./src/shaders/lighting.frag");

  wallTexture = loadTextureRGB("res/wallhue.png");

  setDefaults(&inputs);

}
void GameUpdate() {}
