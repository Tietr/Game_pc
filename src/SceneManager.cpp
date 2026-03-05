#include "SceneManager.h"
void SceneManager::RegisterScene(const std::string &name,
                                 std::unique_ptr<BaseScene> scene) {
  m_scenes[name] = std::move(scene);
}
void SceneManager::Update(float deltaTime) {
  if (m_currentScene == nullptr) {
    return;
  }
  m_currentScene->Update(deltaTime);
  if (m_currentScene->GetName() != m_currentScene->GetNextSceneName()) {
    ChangeScene(m_currentScene->GetNextSceneName());
  }
}
void SceneManager::Render(SDL_Renderer *render) {
  if (m_currentScene == nullptr) {
    return;
  }
  m_currentScene->Render(render);
}
bool SceneManager::ChangeScene(const std::string_view name) {
  if (m_currentScene != nullptr ) {
    m_currentScene->OnExit();
  }
  auto newScene = m_scenes.find(name);
  if (newScene == m_scenes.end()) {
    std::cout << "Scene " << name << " not found" << std::endl;
    return false;
  }
  m_currentScene = newScene->second.get();
  m_currentScene->OnEnter();
  return true;
}
std::string_view SceneManager::GetCurrentSceneName() const {
  if (m_currentScene == nullptr) {
    return "";
  }
  return m_currentScene->GetName();
}
void SceneManager::HandleInput(const SDL_Event &event) {
  if (m_currentScene == nullptr) {
    return;
  }
  m_currentScene->HandleInput(event);
}
const std::vector<std::string_view> SceneManager::GetAllSceneNames() {
  std::vector<std::string_view> names;
  for (auto &scene : m_scenes) {
    names.push_back(scene.first);
  }
  return names;
}
