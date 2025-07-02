//
// Created by Gemini on 2025/06/27.
//

#include "Adrenaline.h"
#include <QDebug>
#include "../Characters/Character.h" // 确保包含 Character.h 的完整定义

Adrenaline::Adrenaline(QGraphicsItem *parent)
    : Consumable(parent, ":/Items/Consumables/Adrenaline/adrenaline_icon.png"), // TODO: 替换为实际的肾上腺素图片路径
    speedBoostMultiplier(1.5), // 速度提升 50%
    healPerSecond(2), // 每秒恢复 2 HP
    durationMs(10000) // 持续 10 秒
{
}

void Adrenaline::applyEffect(Character *character) {
    if (character) {
        character->applyAdrenalineEffect(speedBoostMultiplier, healPerSecond, durationMs);
        qDebug() << "Character used Adrenaline, speed boosted and healing over time.";
    }
}
