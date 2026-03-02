#include "Duck.h"
#include <cmath>
#include <cstdlib>

Duck::Duck(Uint32 id, float x, float y, float s) : x(x), y(y), scale(s) {
  ID = id;
  // 1. 随机速度 (2.0 到 5.0 之间)
  float baseSpeedX = 20.0f;
  float baseSpeedY = 20.0f;

  // 2. 随机初始方向 (1 或 -1)
  vx = baseSpeedX * ((rand() % 2 == 0) ? 1 : -1);
  vy = baseSpeedY * ((rand() % 2 == 0) ? 1 : -1);
}

void Duck::updateMove(float deltaTime) {
  // 这里的 deltaTime 建议乘上去，保证不同帧率速度一致
  // 如果没有全局计时器，暂且直接加 vx, vy
  x += vx * deltaTime;
  y += vy * deltaTime;

  // --- 边界检测 (假设屏幕宽度 800，高度 600) ---
  // 考虑鸭子的大小(约35像素 * scale)，防止身体穿模
  float margin = 30.0f * scale;

  // 左右边界检测
  if (x < margin) {
    x = margin;
    vx *= -1; // 反弹
  } else if (x > 800 - margin) {
    x = 800 - margin;
    vx *= -1; // 反弹
  }

  // 上下边界检测
  if (y < margin) {
    y = margin;
    vy *= -1; // 反弹
  } else if (y > 600 - margin) {
    y = 600 - margin;
    vy *= -1; // 反弹
  }
}

void Duck::draw(SDL_Renderer *renderer) {
  // 现在的 y 直接使用，不再叠加 sin(anim)
  float curY = y;

  // 根据 vx 的正负决定头朝向
  int faceDir = (vx > 0) ? 1 : -1;

  int headX = x + (18 * scale * faceDir);
  int headY = curY - 15 * scale;

  // 1. 绘制碰撞区域/背景 (可选)
  // SDL_Rect rect = {static_cast<int>(x - 35*scale), static_cast<int>(y -
  // 35*scale), (int)(70*scale), (int)(70*scale)};
  // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
  // SDL_RenderFillRect(renderer, &rect);

  // 2. 身体
  DrawUtils::fillCircle(renderer, x, curY, 20 * scale, {255, 255, 0, 255});

  // 3. 头部
  DrawUtils::fillCircle(renderer, headX, headY, 12 * scale, {255, 255, 0, 255});

  // 4. 嘴巴 (位置随 faceDir 改变)
  SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
  SDL_Rect beak = {(int)(headX + (faceDir == 1 ? 5 * scale : -15 * scale)),
                   (int)headY, (int)(10 * scale), (int)(5 * scale)};
  SDL_RenderFillRect(renderer, &beak);

  // 5. 眼睛
  DrawUtils::fillCircle(renderer, headX + (4 * scale * faceDir),
                        headY - 3 * scale, 2 * scale, {0, 0, 0, 255});
}

void Duck::FlashDuck(SDL_Renderer *renderer) {
  // 简单的闪烁矩形反馈
  SDL_Rect rect = {static_cast<int>(x - 20 * scale),
                   static_cast<int>(y - 20 * scale), (int)(40 * scale),
                   (int)(40 * scale)};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &rect);
  SDL_RenderPresent(renderer);
}

