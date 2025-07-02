//
// Created by gerw on 8/20/24.
//

#include "Armor.h"
#include <QDebug> // 用于调试

Armor::Armor(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath),
    durability(-1), maxDurability(-1) // 默认 -1 表示没有耐久度，不会被破坏
{
}

void Armor::mountToParent() {
    Mountable::mountToParent();
    setScale(0.8);
    setPos(-59, -176);
    if (pixmapItem != nullptr) {
        pixmapItem->setPos(0, 0);
    }
    setZValue(1); // 确保护甲在角色模型上方
}

void Armor::unmount() {
    Mountable::unmount();
    setScale(0.8);
    // 当护甲被卸下时，可能需要调整其位置，使其出现在地面上
    if (pixmapItem != nullptr) {
        pixmapItem->setPos(0, -120); // 调整位置
    }
    setZValue(0); // 恢复默认Z值
}

// 新增：护甲减伤的默认实现
int Armor::reduceDamage(int incomingDamage, WeaponType weaponType) {
    Q_UNUSED(weaponType); // 标记参数未使用
    // 默认护甲不提供减伤
    return incomingDamage;
}
