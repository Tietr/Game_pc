#pragma once
#include "BasePhyMove.h"
#include <cmath>

// 反弹模式 (BounceMove)
class BounceMove : public BasePhyMove {
protected:
  void OnHandleBoundary(float w, float h) override {
    // 左右反弹
    if (m_x < m_minX) {
      m_x = (float)m_minX;
      m_vx = std::abs(m_vx); // 向右弹
    } else if (m_x + w > m_maxX) {
      m_x = (float)m_maxX - w;
      m_vx = -std::abs(m_vx); // 向左弹
    }

    // 上下反弹
    if (m_y < m_minY) {
      m_y = (float)m_minY;
      m_vy = std::abs(m_vy); // 向下弹
    } else if (m_y + h > m_maxY) {
      m_y = (float)m_maxY - h;
      m_vy = -std::abs(m_vy); // 向上弹
    }
  }
};

// 穿梭模式 (WrapMove)
class WrapMove : public BasePhyMove {
protected:
  void OnHandleBoundary(float w, float h) override {
    // 左右穿梭
    if (m_x + w < m_minX) {    // 完全消失在左侧
      m_x = (float)m_maxX;     // 从右侧出现
    } else if (m_x > m_maxX) { // 完全消失在右侧
      m_x = (float)m_minX - w; // 从左侧出现
    }

    // 上下穿梭
    if (m_y + h < m_minY) {
      m_y = (float)m_maxY;
    } else if (m_y > m_maxY) {
      m_y = (float)m_minY - h;
    }
  }
};

// 阻挡模式 (ClampMove)
class ClampMove : public BasePhyMove {
protected:
  void OnHandleBoundary(float w, float h) override {
    // 左右阻挡
    if (m_x < m_minX) {
      m_x = (float)m_minX;
      m_vx = 0;
    } else if (m_x + w > m_maxX) {
      m_x = (float)m_maxX - w;
      m_vx = 0;
    }

    // 上下阻挡
    if (m_y < m_minY) {
      m_y = (float)m_minY;
      m_vy = 0;
    } else if (m_y + h > m_maxY) {
      m_y = (float)m_maxY - h;
      m_vy = 0;
    }
  }
};

// 无边界模式 (NoBoundaryMove) —— 无视边界
class NoBoundaryMove : public BasePhyMove {
protected:
  void OnHandleBoundary(float w, float h) override {
  }
};