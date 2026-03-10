#include "UIItem.h"



ButtonWithText::ButtonWithText(const SDL_Rect &rect, float flashDuration,
                               bool canFlash, const std::string &text,
                               SDL_Color color)
    : BaseUIItem(rect, flashDuration, canFlash), m_text(text), m_color(color) {}
PhyMove &AimDuck::GetPhysics() { return m_physics; }
void AimDuck::OnRenderNormal(SDL_Renderer *render, TTF_Font *font) {
  // 现在的 y 直接使用，不再叠加 sin(anim)
  float curY = m_physics.GetY();

  // 根据 m_physics.GetVY() 的正负决定头朝向
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
void AimDuck::OnNormalUpdate(float deltaTime) { m_physics.Update(deltaTime); }
void ButtonWithText::OnRenderNormal(SDL_Renderer *render, TTF_Font *font) {
  DrawUtils::FillRectWithText(render, font, m_rect, m_color, m_text.c_str());
}
