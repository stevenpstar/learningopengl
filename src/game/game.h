#include "../../dep/glad/glad.h"
#include "../../dep/GLFW/glfw3.h"

#ifndef H_GAME
#define H_GAME
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void InitGame();
void GameUpdate();
#endif
