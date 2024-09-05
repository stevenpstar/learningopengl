#include "texture.h"
#include "../../dep/glad/glad.h"
#include "../../dep/stb_image.h"
unsigned int loadTexture(const char *path) {
  stbi_set_flip_vertically_on_load(1);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // texture settings, may separate this out to be configurable later.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  int width, height, channels;
  unsigned char *imageData = stbi_load(path, &width, &height, &channels, 0);
  if (imageData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
   // glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture %s\n", path);
  }

  stbi_image_free(imageData);

  return texture;
}
