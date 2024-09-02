#include "../linmath.h"
#ifndef H_CAMERA
#define H_CAMERA
typedef struct Camera {
  vec3 position;
  vec3 target;
  vec3 direction;
  float yaw;
  float pitch;
  float speed;
  float sensitivity;
  double lastX;
  double lastY;
  mat4x4 proj;
  float fov;
} Camera;
float degToRad(float degree);
Camera createCamera(float *position, float *target, float speed);
void mouseLook(float xoff, float yoff, Camera *camera, float deltaTime);
void setDirection(Camera *camera);
void setProjection(unsigned int shader, const char* uniformName, Camera *camera);
void setFOV(float fov, Camera *camera);
#endif
