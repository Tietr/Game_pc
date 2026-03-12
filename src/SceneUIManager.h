#pragma once

#ifndef __PRFCHWINTRIN_H
#define __PRFCHWINTRIN_H
#endif
#include "BaseUIItem.h"
#include <SDL_ttf.h>
#include <memory>
#include <optional>
#include <vector>

class SceneUIManager {
public:
  // UI管理器模式
  enum class UIManagerMode {
    NormalMode, // 普通模式：可以手动更改渲染模式
    FlashMode   // 闪烁模式：自动闪烁逻辑
  };

  SceneUIManager(UIManagerMode mode = UIManagerMode::FlashMode)
      : m_mode(mode) {}
  ~SceneUIManager() = default;
  void AddItem(std::unique_ptr<BaseUIItem> item);
  void EnableFlash();

  void Render(SDL_Renderer *render, TTF_Font *font);

  void Update(const float deltaTime);

  // 设置渲染模式（仅NormalMode下有效）
  void SetRenderType(RenderType renderType);

  // --- 新增方法：命中检测支持 ---

  /**
   * @brief 获取当前正在闪烁的UI索引
   * @return 当前闪烁的UI索引，如果没有UI在闪烁则返回-1
   */
  int getCurrentFlashingIndex() const;

  /**
   * @brief 检查指定索引的UI是否正在闪烁
   */
  bool isItemFlashing(size_t index) const;

  /**
   * @brief 获取当前闪烁的UI的屏幕区域
   */
  std::optional<SDL_Rect> getCurrentFlashingBounds() const;

  /**
   * @brief 命中当前闪烁的UI
   * @return 被命中的UI索引，如果没有UI在闪烁则返回-1
   */
  int hitCurrentFlashingItem();

  /**
   * @brief 移除指定索引的UI项
   */
  bool removeItem(size_t index);

  /**
   * @brief 获取可闪烁的UI数量
   */
  size_t getFlashableCount() const;

  /**
   * @brief 重置闪烁状态
   */
  void resetFlashState();

  /**
   * @brief 获取当前渲染类型
   */
  RenderType GetRenderType() const { return m_renderType; }

  /**
   * @brief 设置普通UI管理器为None状态
   */
  void SetToNoneState();

private:
  std::vector<std::unique_ptr<BaseUIItem>> m_items;
  RenderType m_renderType = RenderType::Render_Normal;
  size_t m_itemIndexCanFlash = 0;
  UIManagerMode m_mode = UIManagerMode::FlashMode;
  void UpdateNormalLogic(const float &deltaTime);
  void UpdateFlashLogic(const float &deltaTime);
};