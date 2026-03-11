#pragma once
#include "BaseUIItem.h"
#include "PhyMove.h"
#include <string>

class ButtonWithText : public BaseUIItem {
public:
  ButtonWithText(const SDL_Rect &rect, float flashDuration, bool canFlash,
                 const std::string &text, SDL_Color color);

private:
  std::string m_text;
  SDL_Color m_color;

protected:
  virtual void OnRenderNormal(SDL_Renderer *render, TTF_Font *font) override;
};

class AimDuck : public BaseUIItem {
public:
  AimDuck(const SDL_Rect &rect, float flashDuration, bool canFlash)
      : BaseUIItem(rect, flashDuration, canFlash),
        m_physics(PhysicsMode::Bounce) {}
  PhyMove &GetPhysics();
  virtual void OnRenderNormal(SDL_Renderer *render, TTF_Font *font) override;
  virtual void OnNormalUpdate(float deltaTime) override;

  
private:
  PhyMove m_physics;
};

class AnimCloud : public BaseUIItem {
public:
  AnimCloud(const SDL_Rect &rect, float flashDuration, bool canFlash)
      : BaseUIItem(rect, flashDuration, canFlash),
        m_physics(PhysicsMode::Wrap) {}
  PhyMove &GetPhysics();
  virtual void OnRenderNormal(SDL_Renderer *render, TTF_Font *font) override;
  virtual void OnNormalUpdate(float deltaTime) override;

private:
  PhyMove m_physics;
};

class AnimBackground : public BaseUIItem {
public:
  AnimBackground(const SDL_Rect &rect, float flashDuration, bool canFlash)
      : BaseUIItem(rect, flashDuration, canFlash) {}
  virtual void OnRenderNormal(SDL_Renderer *render, TTF_Font *font) override;
  virtual void OnNormalUpdate(float deltaTime) override;
};