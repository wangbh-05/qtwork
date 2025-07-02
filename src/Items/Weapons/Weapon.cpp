//
// Created by Gemini on 2025/06/27.
//

#include "Weapon.h"
#include "../Projectiles/Projectile.h" // 用于创建弹丸
#include <QGraphicsScene> // 用于添加到场景
#include <QDateTime> // 用于时间戳
#include <QDebug> // 用于调试输出
#include "../Characters/Character.h" // 确保包含 Character.h 的完整定义，以便使用其方法和属性
#include <QtMath> // 用于 qCos, qSin, qDegreesToRadians

Weapon::Weapon(QGraphicsItem *parent, const QString &pixmapPath)
    : Item(parent, pixmapPath),
    damage(0),
    remainingUses(-1), // 默认无限次使用
    attackRange(0),
    attackInterval(0),
    lastAttackTime(0),
    type(OtherWeaponType)
{
}

void Weapon::mountToParent() {
    Mountable::mountToParent();
    setScale(0.5); // 示例缩放
    // 根据角色朝向调整武器位置
    // 注意：这里的 parentItem() 已经是 Character
    if (parentItem() && parentItem()->transform().m11() < 0) { // 如果角色向左 (transform.m11() 是X轴缩放因子)
        setPos(-50, -50); // 调整到左手位置
        if (getPixmapItem()) getPixmapItem()->setTransform(QTransform().scale(-1, 1)); // 翻转武器图像 (使用 getter)
    } else { // 角色向右
        setPos(50, -50); // 调整到右手位置
        if (getPixmapItem()) getPixmapItem()->setTransform(QTransform()); // 恢复正常 (使用 getter)
    }
    setZValue(100); // 确保武器在角色上方显示
}

void Weapon::unmount() {
    Mountable::unmount();
    setScale(0.8); // 掉落时的示例缩放，可能与装备时不同
    setParentItem(nullptr); // 解除父子关系，使其成为场景的直接子项
    if (getPixmapItem()) getPixmapItem()->setTransform(QTransform()); // 恢复正常方向 (使用 getter)
    // 假设掉落时位置在角色脚下附近，这里简化处理
    setPos(pos().x(), pos().y() + 50); // 稍微下移，使其在角色脚下
    setZValue(0); // 恢复默认Z值
}

bool Weapon::use() {
    // 冷却时间检查
    if (QDateTime::currentMSecsSinceEpoch() - lastAttackTime < attackInterval) {
        return false; // 还在冷却中
    }

    // 使用次数检查
    if (remainingUses == -1) { // 无限次使用
        lastAttackTime = QDateTime::currentMSecsSinceEpoch(); // 更新上次攻击时间
        return true;
    }
    if (remainingUses > 0) {
        remainingUses--;
        lastAttackTime = QDateTime::currentMSecsSinceEpoch(); // 更新上次攻击时间
        return true;
    }
    return false; // 没有剩余使用次数
}

void Weapon::createProjectile(Character *attacker, const QPointF& targetPos, qreal speed, qreal gravity, const QString& pixmap) {
    if (scene() == nullptr) return; // 确保武器已添加到场景中

    // 计算弹丸的初始位置，使其从武器的实际位置发射
    // attacker->pos() 是角色的世界坐标，pos() 是武器相对于角色的局部坐标
    QPointF startPos = attacker->pos() + pos() + QPointF(getPixmapItem()->boundingRect().center().x(), getPixmapItem()->boundingRect().center().y());

    // 根据角色朝向和目标位置计算弹丸发射角度
    qreal angle;
    QLineF lineToTarget(startPos, targetPos);
    angle = lineToTarget.angle(); // Qt 的角度是顺时针，0度为正X轴

    // 弹丸方向需要根据角色面对的方向进行调整
    // 简单逻辑：如果角色向左，弹丸水平速度分量反向；如果角色向右，保持不变
    // 这种处理方式可能不够精确，但适用于简单的横版游戏
    qreal actualSpeedX = speed;
    if (attacker->transform().m11() < 0) { // 角色向左
        // 如果目标在右侧 (角度在 0-180 之间), 弹丸也应该向左发射
        // 此时需要翻转水平方向的速度分量
        // 如果需要精确的镜像效果，这里可能需要更复杂的角度计算
        if (angle >= 0 && angle <= 180) { // 目标在右半边
            angle = 180 - angle; // 镜像到左半边
        }
    } else { // 角色向右 (默认方向)
        // 如果目标在左侧 (角度在 90-270 之间), 弹丸也应该向右发射
        // 此时需要翻转水平方向的速度分量
        if (angle > 90 && angle < 270) { // 目标在左半边
            angle = 180 + (180 - angle); // 镜像到右半边
        }
    }

    qreal radians = qDegreesToRadians(angle);
    // 修正了 QLineF::fromData 的用法，直接计算速度向量的 X 和 Y 分量
    QPointF velocityVector(speed * qCos(radians), -speed * qSin(radians)); // 调整 Y 分量符号以适应 Qt 屏幕坐标系

    Projectile* projectile = new Projectile(nullptr, pixmap, damage, type);
    projectile->setPos(startPos);
    projectile->setVelocity(velocityVector);
    projectile->setGravity(gravity);
    scene()->addItem(projectile); // 将弹丸添加到场景中
    qDebug() << "Created projectile at" << startPos << "with velocity" << velocityVector;
}
