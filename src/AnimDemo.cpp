#include "UIItem.h"
#include <iostream>

// 演示程序：展示不同Anim类的物理行为
void DemonstrateAnimClasses() {
  std::cout << "=== Anim类物理行为演示 ===\n\n";

  // 创建不同的Anim对象
  SDL_Rect rect = {0, 0, 50, 50};

  // 1. AimDuck - 反弹模式
  std::cout << "1. AimDuck (反弹模式)\n";
  AimDuck duck(rect, 0.5f, false);
  duck.GetPhysics().SetPosition(100, 100);
  duck.GetPhysics().SetVelocity(200, 150);
  duck.GetPhysics().SetLimit(0, 800, 0, 600);

  std::cout << "   初始位置: (100, 100)\n";
  std::cout << "   速度: (200, 150)\n";
  std::cout << "   边界: (0, 800, 0, 600)\n";

  // 模拟几次更新
  for (int i = 0; i < 3; i++) {
    duck.OnNormalUpdate(0.1f);
    std::cout << "   更新后位置: (" << duck.GetPhysics().GetX() << ", "
              << duck.GetPhysics().GetY() << ")\n";
  }

  // 2. AnimCloud - 穿梭模式
  std::cout << "\n2. AnimCloud (穿梭模式)\n";
  AnimCloud cloud(rect, 0.0f, false);
  cloud.GetPhysics().SetPosition(700, 100); // 靠近右边界
  cloud.GetPhysics().SetVelocity(200, 0);   // 向右移动
  cloud.GetPhysics().SetLimit(0, 800, 0, 600);

  std::cout << "   初始位置: (700, 100) - 靠近右边界\n";
  std::cout << "   速度: (200, 0) - 向右移动\n";

  // 模拟几次更新
  for (int i = 0; i < 3; i++) {
    cloud.OnNormalUpdate(0.1f);
    std::cout << "   更新后位置: (" << cloud.GetPhysics().GetX() << ", "
              << cloud.GetPhysics().GetY() << ")\n";

    // 解释穿梭行为
    if (cloud.GetPhysics().GetX() < 0) {
      std::cout << "   → 从右边消失，从左边出现！\n";
    }
  }

  // 3. AnimBackground - 无物理组件
  std::cout << "\n3. AnimBackground (无物理组件)\n";
  AnimBackground background(rect, 0.0f, false);

  std::cout << "   创建成功，没有物理组件\n";
  std::cout << "   OnNormalUpdate() 不会移动位置\n";

  // 模拟更新
  background.OnNormalUpdate(0.1f);
  std::cout << "   更新完成，位置保持不变\n";

  // 4. 动态切换模式演示
  std::cout << "\n4. 动态切换模式演示\n";
  PhyMove physics;
  physics.SetPosition(400, 300);
  physics.SetVelocity(100, 100);

  std::cout << "   初始模式: 反弹模式\n";
  std::cout << "   位置: (" << physics.GetX() << ", " << physics.GetY()
            << ")\n";

  // 切换到穿梭模式
  physics.ChangeToWrapMode();
  std::cout << "   切换到穿梭模式\n";

  physics.Update(0.2f, 50, 50);
  std::cout << "   更新后位置: (" << physics.GetX() << ", " << physics.GetY()
            << ")\n";

  std::cout << "\n=== 演示结束 ===\n";
  std::cout << "\n总结:\n";
  std::cout << "- AimDuck: 使用反弹模式，撞墙会反弹\n";
  std::cout << "- AnimCloud: 使用穿梭模式，出界会从另一边出现\n";
  std::cout << "- AnimBackground: 无物理组件，适合静态背景元素\n";
  std::cout << "- 所有类都保持兼容性，可以动态切换模式\n";
}

// 主函数
int main() {
  DemonstrateAnimClasses();
  return 0;
}