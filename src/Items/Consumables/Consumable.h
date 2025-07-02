//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_CONSUMABLE_H
#define QT_PROGRAMMING_2024_CONSUMABLE_H

#include "../Item.h"
#include "../Characters/Character.h" // 需要 Character 的前向声明或包含

class Character; // 前向声明，避免循环引用

class Consumable : public Item {
public:
    explicit Consumable(QGraphicsItem *parent, const QString &pixmapPath);

    // 纯虚函数，所有派生消耗品必须实现其效果应用逻辑
    virtual void applyEffect(Character *character) = 0;
};


#endif //QT_PROGRAMMING_2024_CONSUMABLE_H
