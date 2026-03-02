#include "Background.h"
#include <iostream>
Background::Background() {

  m_clouds = {
      {400, 120, 1.2f, 5.f}, {600, 190, 0.8f, 3.f}, {90, 180, 1.0f, 4.f}};
}

void Background::Draw(SDL_Renderer *renderer) {

  DrawSky(renderer);
  DrawGrass(renderer);
  DrawCartoonSun(renderer, 650, 100);
  for (auto &cloud : m_clouds) {
    DrawCloud(renderer, cloud.x, cloud.y, cloud.scale);
  }
}

void Background::update(float deltaTime) {
  
  // 更新云朵位置
  for (auto &cloud : m_clouds) {
    cloud.x += cloud.speed * deltaTime;
    if (cloud.x > 850) {
      cloud.x = -100;
    }
  }

}

void Background::DrawSky(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); //蓝色
  SDL_RenderFillRect(renderer, nullptr);
}

void Background::DrawGrass(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); //草绿色
  SDL_Rect grassRect = {0, 400, 800, 200}; 
  SDL_RenderFillRect(renderer, &grassRect);
}

void Background::DrawCartoonSun(SDL_Renderer *renderer, float x, float y) {
  DrawUtils::fillCircle(renderer, x, y, 60, {255, 252, 187}); //浅黄色
  DrawUtils::fillCircle(renderer, x, y, 54, {255, 230, 80}); //橙色
  DrawUtils::fillCircle(renderer, x, y, 50, {255, 190, 0}); //棕色
}

void Background::DrawCloud(SDL_Renderer *renderer, float x, float y,
                           float scale) {
  SDL_Color w = {255, 255, 255, 255}; //白色
  DrawUtils::fillCircle(renderer, x, y, 30 * scale, w);
  DrawUtils::fillCircle(renderer, x - 35 * scale, y + 5 * scale, 20 * scale, w);
  DrawUtils::fillCircle(renderer, x + 35 * scale, y + 5 * scale, 22 * scale, w);
  DrawUtils::fillCircle(renderer, x + 10 * scale, y - 15 * scale, 18 * scale,
                        w);
}

void Background::enableFlash() { m_flash = true; }
void Background::disableFlash() { m_flash = false; }
bool Background::isFlash() {return m_flash; }
void Background::flashScreen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //黑色
    SDL_RenderFillRect(renderer, nullptr);
    SDL_RenderPresent(renderer);
}

