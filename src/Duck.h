#pragma once

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "SDL_render.h"


class Duck {
public:
  float x, y, scale;
  float vx, vy; // X轴和Y轴的速度

  Duck(uint32_t id, float x, float y, float scale);
  ~Duck() = default;

  void updateMove(const float deltaTime);
  void draw(SDL_Renderer *renderer);

  void FlashDuck(SDL_Renderer *renderer, const float delayTime);

private:
  uint32_t ID;
};