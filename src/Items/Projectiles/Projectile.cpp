//
// Created by Gemini on 2025/06/27.
//

#include "Projectile.h"
#include <QDebug>

Projectile::Projectile(QGraphicsItem *parent, const QString &pixmapPath, int damage, WeaponType type)
    : Item(parent, pixmapPath),
    damage(damage),
    weaponType(type),
    gravity(0) // 默认无重力
{
    // 弹丸通常不会被父项管理，因此设为顶级项
    setParentItem(nullptr);
    setZValue(200); // 确保弹丸在角色和武器上方显示
}

void Projectile::processMovement(qint64 deltaTime) {
    // 应用重力（如果大于0）
    velocity.setY(velocity.y() + gravity * deltaTime);

    // 更新位置
    setPos(pos() + velocity * (double)deltaTime);

    // TODO: 可以添加弹丸命中效果、消失计时器等
}
