#pragma once
#include "BaseScene.h"
#include <memory>
#include <string>
#include <unordered_map>

class SceneManager {
public:
  SceneManager() = default;
  ~SceneManager() = default;
  SceneManager(const SceneManager &) = delete;
  SceneManager &operator=(const SceneManager &) = delete;
  SceneManager(SceneManager &&) = default;
  SceneManager &operator=(SceneManager &&) = default;

  void RegisterScene(const std::string &name, std::shared_ptr<BaseScene> scene);
  void Update(float deltaTime);
  void Render(SDL_Renderer *render, TTF_Font *font);
  bool ChangeScene(const std::string &name);
  std::string GetCurrentSceneName() const;
  void HandleInput(const SDL_Event &event);
  std::vector<std::string> GetAllSceneNames();

  // 获取当前场景指针
  std::shared_ptr<BaseScene> GetCurrentScene() { return m_currentScene.lock(); }

private:
  std::weak_ptr<BaseScene> m_currentScene;

  std::unordered_map<std::string, std::shared_ptr<BaseScene>> m_scenes;
};