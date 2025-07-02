//
// Created by Gemini on 2025/06/27.
//

#include "Medkit.h"
#include <QDebug>
#include "../Characters/Character.h" // 确保包含 Character.h 的完整定义

Medkit::Medkit(QGraphicsItem *parent)
    : Consumable(parent, ":/Items/Consumables/Medkit/medkit_icon.png") // TODO: 替换为实际的医疗箱图片路径
{
}

void Medkit::applyEffect(Character *character) {
    if (character) {
        character->heal(character->getMaxHealth() - character->getHealth()); // 恢复全部生命值
        qDebug() << "Character used Medkit, healed to full HP.";
    }
}
