#pragma once
#include <string_view>
#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include <SDL.h>

class BaseScene {
public:
  ~BaseScene() = default;
  virtual void OnInit(const std::string_view &name) = 0;
  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(SDL_Renderer *render) = 0;
  virtual void HandleInput(const SDL_Event &event) = 0;
  inline virtual std::string_view GetName() const { return m_name; }
  inline virtual std::string_view GetNextSceneName() const { return m_nextSceneName; }

protected:
  std::string_view m_name = "BaseScene";
  std::string_view m_nextSceneName = "BaseScene";
  
};