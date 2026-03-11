#include "GameScene.h"
#include "SceneUIManager.h"
#include "UIItem.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

void GameScene::Update(float deltaTime) {
  // 检测游戏状态
  // 分析数据
  // 更新所有UIItem

  if (m_uiManager) {
    m_uiManager->Update(deltaTime);
  }

  // 处理游戏状态逻辑
  if (m_gameState == GameState::Flashing) {
    // 闪烁状态处理
    if (m_uiManager) {
      m_uiManager->EnableFlash();
    }
    m_gameState = GameState::Running;
  }
}
void GameScene::Render(SDL_Renderer *render, TTF_Font *font) {
  // 通过SceneUIManager渲染所有UIItem
  // 注意：SceneUIManager会按照添加顺序渲染，所以AnimBackground在最底层
  if (m_uiManager) {
    m_uiManager->Render(render, font);
  }
}
void GameScene::OnExit() {
  std::cout << "Exiting GameScene" << std::endl;
  m_gameState = GameState::None;
}
void GameScene::OnEnter() {
  BaseScene::OnEnter();
  std::cout << "Entering GameScene" << std::endl;
}
void GameScene::HandleInput(const SDL_Event &event) {
  switch (event.type) {
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_q) {
      m_nextSceneName = "mainMenu";
    }
    if (event.key.keysym.sym == SDLK_SPACE) {
      m_gameState = GameState::Flashing;
    }
    break;
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    std::cout << "Clicked" << std::endl;
  }
}
void GameScene::OnInit(const std::string &name) {
  m_name = name;
  m_nextSceneName = name;
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  m_uiManager = std::make_unique<SceneUIManager>();
  m_gameState = GameState::Running;
  CreateAnimBackground();
  CreateAnimClouds();
  CreateAimDucks();
}
void GameScene::CreateAnimBackground() {
  SDL_Rect bgRect = {0, 0, 800, 600};
  auto background = std::make_unique<AnimBackground>(bgRect, 0.0f, false);
  m_uiManager->AddItem(std::move(background));
  std::cout << "AnimBackground created (no flash, bottom layer)" << std::endl;
}
void GameScene::CreateAnimClouds() {
  const int cloudWidth = 80;
  const int cloudHeight = 40;
  std::vector<SDL_Point> cloudPositions = {
      {100, 30}, 
      {350, 80}, 
      {700, 60} 
  };
  std::vector<SDL_Point> cloudSpeeds = {
      {8, 0}, 
      {14, 0}, 
      {17, 0} 
  };

  for (int i = 0; i < 3; i++) {
    SDL_Rect cloudRect = {cloudPositions[i].x, cloudPositions[i].y, cloudWidth,
                          cloudHeight};
    auto cloud = std::make_unique<AnimCloud>(cloudRect, 0.0f, false);
    cloud->GetPhysics().SetPosition(static_cast<float>(cloudPositions[i].x),
                                    static_cast<float>(cloudPositions[i].y));
    cloud->GetPhysics().SetVelocity(static_cast<float>(cloudSpeeds[i].x),
                                    static_cast<float>(cloudSpeeds[i].y));
    cloud->GetPhysics().SetLimit(-20, 900, 0, 600);
    m_uiManager->AddItem(std::move(cloud));
    std::cout << "AnimCloud " << i + 1 << " created at (" << cloudPositions[i].x
              << ", " << cloudPositions[i].y << ") with speed ("
              << cloudSpeeds[i].x << ", 0)" << std::endl;
  }
}
void GameScene::CreateAimDucks() {
  const int duckWidth = 70;
  const int duckHeight = 70;
  std::vector<SDL_Point> duckPositions = {
      {200, 300}, 
      {400, 250}, 
      {600, 350}  
  };
  std::vector<SDL_Point> duckSpeeds = {
      {120, 90},  
      {-100, 120}, 
      {80, -110}
  };
  m_duckPointers.clear();

  for (int i = 0; i < 3; i++) {
    SDL_Rect duckRect = {duckPositions[i].x, duckPositions[i].y, duckWidth,
                         duckHeight};

    auto duck = std::make_unique<AimDuck>(duckRect, 0.5f, true);
    duck->GetPhysics().SetPosition(static_cast<float>(duckPositions[i].x),
                                   static_cast<float>(duckPositions[i].y));
    duck->GetPhysics().SetVelocity(static_cast<float>(duckSpeeds[i].x),
                                   static_cast<float>(duckSpeeds[i].y));
    duck->GetPhysics().SetLimit(30, 850, 30, 450);
    AimDuck *duckPtr = duck.get();
    m_duckPointers.push_back(duckPtr);
    m_uiManager->AddItem(std::move(duck));
    std::cout << "AimDuck " << i + 1 << " created at (" << duckPositions[i].x
              << ", " << duckPositions[i].y << ") with speed ("
              << duckSpeeds[i].x << ", " << duckSpeeds[i].y << ")" << std::endl;
  }
}
void GameScene::AddDuck(int x, int y) {
  SDL_Rect duckRect = {x, y, 40, 40};
  float speedX = static_cast<float>((std::rand() % 200) - 100); 
  float speedY = static_cast<float>((std::rand() % 200) - 100); 
  auto duck = std::make_unique<AimDuck>(duckRect, 0.5f, true);
  duck->GetPhysics().SetVelocity(speedX, speedY);
  duck->GetPhysics().SetLimit(0, 800, 0, 600);
  AimDuck *duckPtr = duck.get();
  m_duckPointers.push_back(duckPtr);
  m_uiManager->AddItem(std::move(duck));
  std::cout << "Duck added at (" << x << ", " << y << ") with speed (" << speedX
            << ", " << speedY << ")" << std::endl;
}

void GameScene::RemoveDuck(int index) {
  if (index >= 0 && index < static_cast<int>(m_duckPointers.size())) {
    // 注意：这里只是从指针列表中移除，实际的UIItem由SceneUIManager管理
    // 在实际实现中，需要SceneUIManager提供移除功能
    std::cout << "Duck removal at index " << index << " (not fully implemented)"
              << std::endl;
    m_duckPointers.erase(m_duckPointers.begin() + index);
  }
}

void GameScene::ClearAllDucks() {
  std::cout << "Clearing all ducks (not fully implemented)" << std::endl;
  m_duckPointers.clear();
}
