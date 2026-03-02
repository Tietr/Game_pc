#pragma once
#include "drawUtils.h"
#include <vector>

class Duck {
public:
  float x, y, scale;
  float vx, vy; // X轴和Y轴的速度

  Duck(Uint32 id, float x, float y, float scale);
  ~Duck() = default;

  void updateMove(float deltaTime);
  void draw(SDL_Renderer *renderer);

  void FlashDuck(SDL_Renderer *renderer);

private:
  Uint32 ID;
};