#include <stdbool.h>
#ifndef H_PRIMITIVES
#define H_PRIMITIVES
typedef struct P_CUBE {
  float posX;
  float posY;
  float posZ;
  float vertices[180];
} P_CUBE;

// this is game specific but oh well
// Cube without bottom and top
typedef struct Wall {
  float posX;
  float posY;
  float posZ;
  float vertices[144];
} Wall;

typedef struct Animation {
  int state;
  int startFrame;
  int endFrame;
} Animation;

typedef struct Sprite {
  float posX;
  float posY;
  float posZ;
  unsigned int texture;
  int frameWidth;
  int frameHeight;
  int texWidth;
  int texHeight;
  int frameCount;
  int currentFrame;
  float frameTime;
  float data[30]; // Formatted: vert x, y, z, tex coords x, y
  int indices[6];
} Sprite;

typedef struct P_CUBE_LIGHT {
  float posX;
  float posY;
  float posZ;
  float vertices[108];
} P_CUBE_LIGHT;

P_CUBE createCube(unsigned int VBO);
void updateCubeVBO(unsigned int VBO, P_CUBE *cube);
Wall createWall(unsigned int VBO);
P_CUBE_LIGHT createCubeLight(unsigned int VBO, float x, float y, float z);
Sprite createSprite(unsigned int VBO, const char *tex);
Sprite createAnimatedSprite(unsigned int VBO, unsigned int EBO, float x, float y, float z, const char *tex, int frameWidth,
    int frameHeight, int texWidth, int texHeight);
void SetFrame(Sprite *sprite, int frame, unsigned int VBO, bool flipHorizontal);
void setCubeLight(unsigned int VBO, float *vertices);
#endif
