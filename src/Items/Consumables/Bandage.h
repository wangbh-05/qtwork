//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_BANDAGE_H
#define QT_PROGRAMMING_2024_BANDAGE_H

#include "Consumable.h"

class Bandage : public Consumable {
public:
    explicit Bandage(QGraphicsItem *parent = nullptr);
    void applyEffect(Character *character) override;

private:
    int healAmount;
};


#endif //QT_PROGRAMMING_2024_BANDAGE_H
