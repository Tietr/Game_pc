#pragma once
#include <cstdlib>
class PhyMove {
public: // 改为 public，方便外部（或宿主类）调用
  PhyMove() : m_vx(0), m_vy(0), m_x(400), m_y(300) { SetLimit(0, 800, 0, 600); }

  void SetLimit(int minX, int maxX, int minY, int maxY) {
    limitXMin = minX;
    limitXMax = maxX;
    limitYMin = minY;
    limitYMax = maxY;
  }

  void SetPosition(float x, float y) {
    m_x = x;
    m_y = y;
  }
  void SetVelocity(float vx, float vy) {
    m_vx = vx;
    m_vy = vy;
  }

  float GetX() const { return m_x; }
  float GetY() const { return m_y; }
  float GetVX() const { return m_vx; }
  float GetVY() const { return m_vy; }

  void Update(const float deltaTime, float objectWidth = 0.0f,
              float objectHeight = 0.0f) {
    m_x += m_vx * deltaTime;
    m_y += m_vy * deltaTime;
    if (m_x < limitXMin) {
      m_x = (float)limitXMin;
      m_vx = std::abs(m_vx); // 向右弹
    } else if (m_x + objectWidth > limitXMax) {
      m_x = (float)limitXMax - objectWidth;
      m_vx = -std::abs(m_vx); // 向左弹
    }
    if (m_y < limitYMin) {
      m_y = (float)limitYMin;
      m_vy = std::abs(m_vy); // 向下弹
    } else if (m_y + objectHeight > limitYMax) {
      m_y = (float)limitYMax - objectHeight;
      m_vy = -std::abs(m_vy); // 向上弹
    }
  }
private:
  float m_vx, m_vy;
  float m_x, m_y;
  int limitXMin, limitXMax, limitYMin, limitYMax;

};
