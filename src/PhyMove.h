#pragma once
#include "BasePhyMove.h"
#include "PhyMoveTypes.h"
#include <memory>

// 物理模式枚举
enum class PhysicsMode {
  Bounce,    // 反弹模式
  Wrap,      // 穿梭模式
};

// PhyMove 现在作为一个兼容性包装类，默认使用反弹模式
// 这样可以保持现有代码的兼容性
class PhyMove {
public:
  // 默认构造函数：使用反弹模式
  PhyMove() : PhyMove(PhysicsMode::Bounce) {}

  // 指定模式的构造函数
  PhyMove(PhysicsMode mode) {
    switch (mode) {
    case PhysicsMode::Bounce:
      m_physics = std::make_unique<BounceMove>();
      break;
    case PhysicsMode::Wrap:
      m_physics = std::make_unique<WrapMove>();
      break;
    }
    m_physics->SetPosition(400, 300);
  }

  // 保持原有接口的兼容性
  void SetLimit(int minX, int maxX, int minY, int maxY) {
    m_physics->SetLimit(minX, maxX, minY, maxY);
  }

  void SetPosition(float x, float y) { m_physics->SetPosition(x, y); }

  void SetVelocity(float vx, float vy) { m_physics->SetVelocity(vx, vy); }

  float GetX() const { return m_physics->GetX(); }
  float GetY() const { return m_physics->GetY(); }
  float GetVX() const { return m_physics->GetVX(); }
  float GetVY() const { return m_physics->GetVY(); }

  void Update(const float deltaTime, float objectWidth = 0.0f,
              float objectHeight = 0.0f) {
    m_physics->Update(deltaTime, objectWidth, objectHeight);
  }

  // 新增功能：动态更换物理行为
  void ChangeToBounceMode() { ChangePhysicsMode<BounceMove>(); }

  void ChangeToWrapMode() { ChangePhysicsMode<WrapMove>(); }

  BasePhyMove *GetPhysics() { return m_physics.get(); }

private:
  template <typename T> void ChangePhysicsMode() {
    float x = m_physics->GetX();
    float y = m_physics->GetY();
    float vx = m_physics->GetVX();
    float vy = m_physics->GetVY();

    auto newPhysics = std::make_unique<T>();
    newPhysics->SetPosition(x, y);
    newPhysics->SetVelocity(vx, vy);
    m_physics = std::move(newPhysics);
  }

private:
  std::unique_ptr<BasePhyMove> m_physics;
};
