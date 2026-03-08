#pragma once
#include "BaseScene.h"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class SceneManager {
public:
  SceneManager() = default;
  ~SceneManager() = default;

  // 禁止拷贝
  SceneManager(const SceneManager &) = delete;
  SceneManager &operator=(const SceneManager &) = delete;

  // 允许移动
  SceneManager(SceneManager &&) = default;
  SceneManager &operator=(SceneManager &&) = default;

  void RegisterScene(const std::string &name, std::shared_ptr<BaseScene> scene);
  void Update(float deltaTime);
  void Render(SDL_Renderer *render);
  bool ChangeScene(const std::string &name);
  std::string GetCurrentSceneName() const;
  void HandleInput(const SDL_Event &event);
  std::vector<std::string> GetAllSceneNames();

private:
  std::weak_ptr<BaseScene> m_currentScene;

  std::unordered_map<std::string, std::shared_ptr<BaseScene>> m_scenes;
};