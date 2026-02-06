#include "UI.h"

UI::UI() : score(0), maxScore(10) {}

void UI::addScore() {
  if (score < maxScore)
    score++;
}

void UI::reset() {
  score = 0;
}
void UI::draw(SDL_Renderer *renderer) {
  // 1. 开启 Alpha 混合模式以支持半透明
  // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // 2. 绘制底部信息栏
  SDL_SetRenderDrawColor(renderer, panelColor.r, panelColor.g, panelColor.b,
                         panelColor.a);

  SDL_Rect buttomBar = {0, 550, 800, 50};
  SDL_RenderFillRect(renderer, &buttomBar);

  // 3. 绘制“分数槽”（空槽）
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150); // 浅白色底
  for (int i = 0; i < maxScore; i++) {
    SDL_Rect slot = {20 + i * 35, 567, 25, 16};
    SDL_RenderDrawRect(renderer, &slot);
  }

  // 4. 填充已得分数（亮黄色的能量块）
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
  for (int i = 0; i < score; i++) {
    SDL_Rect fill = {22 + i * 35, 17, 21, 16};
    SDL_RenderFillRect(renderer, &fill);
  }

  // 5. 绘制一个迷你的“鸭子图标”作为提示
  drawMiniDuck(renderer, 380, 575);
}

void UI::drawMiniDuck(SDL_Renderer *renderer, int x, int y) {
  SDL_Color yellow = {255, 255, 0, 255};
  DrawUtils::fillCircle(renderer, x, y, 8, yellow); // 头
  SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
  SDL_Rect beak = {x + 4, y, 6, 3}; // 嘴
  SDL_RenderFillRect(renderer, &beak);
}