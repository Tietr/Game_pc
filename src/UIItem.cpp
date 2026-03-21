#include "UIItem.h"
#include "DrawUtils.h"
#include <SDL_ttf.h>

ButtonWithText::ButtonWithText(const SDL_Rect &rect, float flashDuration,
                               bool canFlash, const std::string &text,
                               SDL_Color color, std::function<void()> deadCall)
    : BaseUIItem(rect, flashDuration, canFlash), m_text(text), m_color(color),
      m_deadCall(deadCall) {}

PhyMove &AimDuck::GetPhysics() { return m_physics; }

void AimDuck::OnRenderNormal(SDL_Renderer *render, TTF_Font *font) {
  float curY = m_physics.GetY();
  int faceDir = (m_physics.GetVY() > 0) ? 1 : -1;

  int headX = static_cast<int>(m_physics.GetX() + (18 * faceDir));
  int headY = static_cast<int>(curY - 15);

  // 2. 身体
  DrawUtils::fillCircle(render, static_cast<const int>(m_physics.GetX()),
                        static_cast<const int>(curY),
                        static_cast<const int>(20), {255, 255, 0, 255});

  // 3. 头部
  DrawUtils::fillCircle(render, headX, headY, static_cast<const int>(12),
                        {255, 255, 0, 255});

  // 4. 嘴巴 (位置随 faceDir 改变)
  SDL_SetRenderDrawColor(render, 255, 165, 0, 255);
  SDL_Rect beak = {(int)(headX + (faceDir == 1 ? 5 : -15)), (int)headY,
                   (int)(10), (int)(5)};
  SDL_RenderFillRect(render, &beak);

  // 5. 眼睛
  DrawUtils::fillCircle(render, static_cast<const int>(headX + (4 * faceDir)),
                        static_cast<const int>(headY - 3),
                        static_cast<const int>(2), {0, 0, 0, 255});
}

void AimDuck::OnNormalUpdate(float deltaTime) {
  m_physics.Update(deltaTime, m_rect.w, m_rect.h); // 鸭子大约40x40像素
  int bios = 35;                                   // 碰撞边界偏移，防止鸭子穿模
  m_rect = {static_cast<int>(m_physics.GetX() - bios),
            static_cast<int>(m_physics.GetY() - bios), m_rect.w, m_rect.h};
}

void ButtonWithText::OnRenderNormal(SDL_Renderer *render, TTF_Font *font) {
  DrawUtils::FillRectWithText(render, font, m_rect, m_color, m_text.c_str());
}

// AnimCloud 实现
void AnimCloud::OnRenderNormal(SDL_Renderer *render, TTF_Font *font) {
  SDL_Color w = {255, 255, 255, 255}; // 白色
  DrawUtils::fillCircle(render, static_cast<const int>(m_rect.x),
                        static_cast<const int>(m_rect.y),
                        static_cast<const int>(30), w);
  DrawUtils::fillCircle(render, static_cast<const int>(m_rect.x - 35),
                        static_cast<const int>(m_rect.y + 5),
                        static_cast<const int>(20), w);

  DrawUtils::fillCircle(render, static_cast<const int>(m_rect.x + 35),
                        static_cast<const int>(m_rect.y + 5),
                        static_cast<const int>(22), w);

  DrawUtils::fillCircle(render, static_cast<const int>(m_rect.x + 10),
                        static_cast<const int>(m_rect.y - 15),
                        static_cast<const int>(18), w);
}

void AnimCloud::OnNormalUpdate(float deltaTime) {
  m_physics.Update(deltaTime, m_rect.w, m_rect.h);
  m_rect = {static_cast<int>(m_physics.GetX()),
            static_cast<int>(m_physics.GetY()), m_rect.w, m_rect.h};
}

// AnimBackground 实现
void AnimBackground::OnRenderNormal(SDL_Renderer *render, TTF_Font *font) {
  // sky
  SDL_SetRenderDrawColor(render, 135, 206, 235, 255); // 蓝色
  SDL_RenderFillRect(render, nullptr);
  // grass
  SDL_SetRenderDrawColor(render, 34, 139, 34, 255); // 草绿色
  SDL_Rect grassRect = {0, 400, 800, 200};
  SDL_RenderFillRect(render, &grassRect);
  // CartoonSun
  int x = 650;
  int y = 100;
  DrawUtils::fillCircle(render, x, y, 60, {255, 252, 187}); // 浅黄色
  DrawUtils::fillCircle(render, x, y, 54, {255, 230, 80});  // 橙色
  DrawUtils::fillCircle(render, x, y, 50, {255, 190, 0});   // 棕色
}

void AnimBackground::OnNormalUpdate(float deltaTime) {
  // AnimBackground 不需要物理更新，可以留空或添加其他动画逻辑
  // 例如：可以添加颜色渐变、透明度变化等非物理动画
}
PhyMove &AnimCloud::GetPhysics() { return m_physics; }
