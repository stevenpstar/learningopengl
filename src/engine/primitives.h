#ifndef H_PRIMITIVES
#define H_PRIMITIVES
typedef struct P_CUBE {
  float posX;
  float posY;
  float posZ;
  float vertices[180];
} P_CUBE;

typedef struct Animation {
  int id;
  int startFrame;
  int frameCount;
  int active;
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
P_CUBE_LIGHT createCubeLight(unsigned int VBO, float x, float y, float z);
Sprite createSprite(unsigned int VBO, const char *tex);
Sprite createAnimatedSprite(unsigned int VBO, unsigned int EBO, float x, float y, float z, const char *tex, int frameWidth,
    int frameHeight, int texWidth, int texHeight);
void SetFrame(Sprite *sprite, int frame, unsigned int VBO);

#endif
