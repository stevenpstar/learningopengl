#include "../../dep/glad/glad.h"
#include "tile.h"
#include "../engine/texture.h"
#include <math.h>

unsigned int createWorld(int *tiles, const char *tilesFile, GLubyte *pixelData) {
  unsigned int texture = loadTexture(tilesFile);
  for (int i=0;i<1024;i++) {

    int elementsPerRow = 32 * 3;
    int y = floor((float)i / 32);
    int x = i - (y * 32);
    // level tex coordinates (move this to function later)
    int tr = y * elementsPerRow;
    int tc = x * 3;
    int r = pixelData[tr + tc];
    int g = pixelData[tr + tc + 1];
    int b = pixelData[tr + tc + 2];
    int tile = 0;
    // TODO extract this into game specific file, tile may be moved to engine?
    //  ?? maybe
    if (r == 127 && g == 127 && b == 127) {
      tile = 0;
    } 
    else if (r == 136 && g == 0 && b == 21) {
      tile = 1;
    }
    else {
      tile = 34;
    }
    tiles[i] = tile;
  }
  return texture;
}

void setTileData(int i, int tileSize, int texWidth, int texHeight, GLubyte *pixelData, float *data, unsigned int VBO) {

  float fw = tileSize * i; // this should be tile id
  int rowNum = floor(fw / texWidth);
  int colNum = (fw - (rowNum * texWidth)) / tileSize;
//  printf("rowNum: %d\n", rowNum);
//  printf("colNum: %d\n", colNum);
  float row = ((float)rowNum * tileSize) / texHeight;
  float col = (fw - (rowNum * tileSize));// / texWidth;
  float normWidth = (float)tileSize / (float)texWidth;
  float normHeight = (float)tileSize / (float)texHeight;

  float topY = 1.0 - normHeight * rowNum;
  float bottomY = 1.0 - (normHeight * rowNum) - normHeight;
  float left = 0.0 + normWidth * colNum;
  float right = 0.0 + (normWidth * colNum) + normWidth;
  // bottom left
  data[6] = left; // done
  data[7] = bottomY;// done
  // duplicate vertex bottom l
  data[46] = left;
  data[47] = bottomY;

  // bottom right
  data[14] = right; // done
  data[15] = bottomY; // done

  // top right
  data[22] = right; // done
  data[23] = topY; // done
  data[30] = right; // done
  data[31] = topY; // done

  // top left
  data[38] = left; // done
  data[39] = topY; // done

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void resetTileTexCoords(float *data, unsigned int VBO) {
  float topY = 1.0;
  float bottomY = 0.0;
  float left = 0.0;
  float right = 1.0;
  // bottom left
  data[6] = left; // done
  data[7] = bottomY;// done
  // duplicate vertex bottom l
  data[46] = left;
  data[47] = bottomY;

  // bottom right
  data[14] = right; // done
  data[15] = bottomY; // done

  // top right
  data[22] = right; // done
  data[23] = topY; // done
  data[30] = right; // done
  data[31] = topY; // done

  // top left
  data[38] = left; // done
  data[39] = topY; // done

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void getTileFromPosition(float x, float y, int *pos) {
  float tileDim = 0.5f;
  int tileX = floor(x / tileDim);
  int tileY = floor(y / tileDim);
  pos[0] = tileX;
  pos[1] = tileY;
}

int getIndexFromTile(int x, int y, int mapWidth) {
  return (y * mapWidth) + x;
}
