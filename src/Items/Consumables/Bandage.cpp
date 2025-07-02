//
// Created by Gemini on 2025/06/27.
//

#include "Bandage.h"
#include <QDebug>
#include "../Characters/Character.h" // 确保包含 Character.h 的完整定义

Bandage::Bandage(QGraphicsItem *parent)
    : Consumable(parent, ":/Items/Consumables/Bandage/bandage_icon.png"), // TODO: 替换为实际的绷带图片路径
    healAmount(20)
{
}

void Bandage::applyEffect(Character *character) {
    if (character) {
        character->heal(healAmount);
        qDebug() << "Character used Bandage, healed" << healAmount << "HP.";
    }
}
