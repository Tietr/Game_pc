#pragma once
#include "BasePhyMove.h"
#include <cmath>

class BounceMove : public BasePhyMove {
protected:
  void OnHandleBoundary(float w, float h) override {
    // 左右反弹
    if (m_x < m_minX) {
      m_x = (float)m_minX;
      m_vx = std::abs(m_vx); 
    } else if (m_x + w > m_maxX) {
      m_x = (float)m_maxX - w;
      m_vx = -std::abs(m_vx); 
    }

    // 上下反弹
    if (m_y < m_minY) {
      m_y = (float)m_minY;
      m_vy = std::abs(m_vy); 
    } else if (m_y + h > m_maxY) {
      m_y = (float)m_maxY - h;
      m_vy = -std::abs(m_vy);
    }
  }
};


class WrapMove : public BasePhyMove {
protected:
  void OnHandleBoundary(float w, float h) override {

    if (m_x + w < m_minX) {  
      m_x = (float)m_maxX;    
    } else if (m_x > m_maxX) { 
      m_x = (float)m_minX - w;
    }

    if (m_y + h < m_minY) {
      m_y = (float)m_maxY;
    } else if (m_y > m_maxY) {
      m_y = (float)m_minY - h;
    }
  }
};
