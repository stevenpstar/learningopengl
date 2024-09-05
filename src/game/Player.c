#include "Player.h"
#include <stdbool.h>

void Animate(Player *player, Animation anim, bool loop, float deltaTime, unsigned int VBO) {
  int startFrame = anim.startFrame;
  int endFrame = anim.endFrame;
  float frameTime = player->framerate / 60.0f;
  bool nextFrame = false;
  bool flipHoriz = false;
  // This is stupid but ye
  if (anim.state == RUN_LEFT || anim.state == IDLE_LEFT) {
    flipHoriz = true;
    //printf("FLIP!");
  }
  if (player->sprite->currentFrame < startFrame || player->sprite->currentFrame > endFrame) {
    player->sprite->currentFrame = startFrame;
    player->frameTimer = 0.0f;
    SetFrame(player->sprite, player->sprite->currentFrame, VBO, flipHoriz);
  }
  player->frameTimer += deltaTime;
  if (player->frameTimer >= frameTime) {
    nextFrame = true;
    player->frameTimer -= frameTime;
  }
  if (!nextFrame) {
    return;
  }
  if (player->sprite->currentFrame == endFrame) {
    player->sprite->currentFrame = startFrame;
  } else {
    player->sprite->currentFrame++;
  }

  SetFrame(player->sprite, player->sprite->currentFrame, VBO, flipHoriz);
}
