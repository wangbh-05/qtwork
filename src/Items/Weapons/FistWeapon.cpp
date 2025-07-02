//
// Created by Gemini on 2025/06/27.
//

#include "FistWeapon.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QLineF> // 用于距离计算
#include "../Characters/Character.h" // 确保包含 Character.h 的完整定义

FistWeapon::FistWeapon(QGraphicsItem *parent)
    : Weapon(parent, "") // 拳头没有图片，pixmapPath 为空
{
    damage = 10; // 最小伤害
    remainingUses = -1; // 无限使用
    attackRange = 50; // 近战范围
    attackInterval = 500; // 攻击间隔 0.5 秒
    type = FistType;
}

void FistWeapon::attack(Character *attacker, const QPointF& targetPos) {
    Q_UNUSED(targetPos); // 拳头攻击不依赖 targetPos，此处标记为未使用
    qDebug() << "Fist attack by" << attacker->pos();

    if (scene() == nullptr) return; // 确保武器在场景中

    // 遍历场景中所有物品，寻找可能的受击角色 (除了攻击者自己)
    for (QGraphicsItem* item : scene()->items()) {
        if (auto targetCharacter = dynamic_cast<Character*>(item)) {
            if (targetCharacter == attacker) continue; // 不攻击自己

            // 检查目标是否在攻击范围内
            qreal distance = QLineF(attacker->pos(), targetCharacter->pos()).length();
            if (distance <= attackRange) {
                // 检查方向：如果角色向右，目标应该在右边；如果角色向左，目标应该在左边
                bool attackerFacingRight = attacker->transform().m11() > 0;
                bool targetIsToRight = targetCharacter->pos().x() > attacker->pos().x();

                if ((attackerFacingRight && targetIsToRight) || (!attackerFacingRight && !targetIsToRight)) {
                    qDebug() << "Fist hit" << targetCharacter->pos();
                    if (targetCharacter->isAlive()) { // 确保目标存活才造成伤害
                        targetCharacter->takeDamage(damage, type); // 目标受到伤害
                    }
                    // TODO: 播放命中音效或动画
                    return; // 命中一个目标后结束攻击
                }
            }
        }
    }
}
