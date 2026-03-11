#pragma once

class BasePhyMove {
public:
  BasePhyMove(float x = 0, float y = 0, float vx = 0, float vy = 0)
      : m_x(x), m_y(y), m_vx(vx), m_vy(vy) {
    SetLimit(0, 800, 0, 600);
  }

  virtual ~BasePhyMove() = default;

  // 更新位置的核心流程
  void Update(float deltaTime, float objW = 0, float objH = 0) {
    // 1. 基础物理移动
    m_x += m_vx * deltaTime;
    m_y += m_vy * deltaTime;

    // 2. 调用边界处理勾子（由子类具体实现）
    OnHandleBoundary(objW, objH);
  }

  // 配置接口
  void SetLimit(int minX, int maxX, int minY, int maxY) {
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
  }
  void SetVelocity(float vx, float vy) {
    m_vx = vx;
    m_vy = vy;
  }
  void SetPosition(float x, float y) {
    m_x = x;
    m_y = y;
  }

  // 获取位置供渲染使用
  float GetX() const { return m_x; }
  float GetY() const { return m_y; }
  float GetVX() const { return m_vx; }
  float GetVY() const { return m_vy; }

protected:
  // --- 虚函数勾子 ---
  // 定义为纯虚函数 (= 0)，强制要求子类必须决定边界行为
  virtual void OnHandleBoundary(float w, float h) = 0;

protected:
  float m_x, m_y;
  float m_vx, m_vy;
  int m_minX, m_maxX, m_minY, m_maxY;
};