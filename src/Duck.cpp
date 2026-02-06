#include "Duck.h"

Duck::Duck(float x, float y, float s) : x(x), y(y), scale(s) {
  speed = 1.0f + (rand() % 10) / 5.0f;
  dir = (rand() % 2 == 0) ? 1 : -1;
  anim = (rand() % 100) / 10.0f;
}

void Duck::draw(SDL_Renderer *renderer) {
  float curY = y + sin(anim) * 5;
  int headX = x + (18 * scale * dir);
  int headY = curY - 15 * scale;

  // 身体
  DrawUtils::fillCircle(renderer, x, curY, 20 * scale, {255, 255, 0, 255});
  // 头部
  DrawUtils::fillCircle(renderer, headX, headY, 12 * scale, {255, 255, 0, 255});
  // 嘴巴
  SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
  SDL_Rect beak = {(int)(headX + (dir == 1 ? 5 * scale : -15 * scale)),
                   (int)headY, (int)(10 * scale), (int)(5 * scale)};
  SDL_RenderFillRect(renderer, &beak);
  // 眼睛
  DrawUtils::fillCircle(renderer, headX + (4 * scale * dir), headY - 3 * scale,
                        2 * scale, {0, 0, 0, 255});
}

void Duck::update(float deltaTime) {
  x += speed * dir;
  anim += 0.2f;
  if (x > 750 || x < 50)
    dir *= -1;
}

void Duck::FlashDuck(SDL_Renderer *renderer) {
    // 画白色正方形
    SDL_Rect rect = {static_cast<int>(x - 20), static_cast<int>(y - 20), 40,
                     40};
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
  
}