#pragma once
#include "drawUtils.h"
#include <vector>

class Duck
{
public:
  float x, y, scale;
  float speed;
  int dir;
  float anim;

  Duck(float x, float y, float scale);
  ~Duck() = default;

  void update(float deltaTime);
  void draw(SDL_Renderer *renderer);

  void FlashDuck(SDL_Renderer *renderer);
};