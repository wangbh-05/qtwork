//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_FISTWEAPON_H
#define QT_PROGRAMMING_2024_FISTWEAPON_H

#include "Weapon.h"

class FistWeapon : public Weapon {
public:
    explicit FistWeapon(QGraphicsItem *parent);
    void attack(Character *attacker, const QPointF& targetPos) override;
};


#endif //QT_PROGRAMMING_2024_FISTWEAPON_H
