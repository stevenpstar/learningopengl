#ifndef H_PRIMITIVES
#define H_PRIMITIVES
typedef struct P_CUBE {
  float posX;
  float posY;
  float posZ;
  float vertices[180];
} P_CUBE;
P_CUBE createCube(unsigned int VBO);
#endif
