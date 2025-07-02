//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_PROJECTILE_H
#define QT_PROGRAMMING_2024_PROJECTILE_H

#include "../Item.h"
#include "../Weapons/Weapon.h" // 确保包含 Weapon.h，以便 WeaponType 可用
#include <QPointF>

class Projectile : public Item {
public:
    explicit Projectile(QGraphicsItem *parent, const QString &pixmapPath, int damage, WeaponType type);

    void processMovement(qint64 deltaTime);

    [[nodiscard]] const QPointF &getVelocity() const { return velocity; }
    void setVelocity(const QPointF &newVelocity) { velocity = newVelocity; }

    [[nodiscard]] qreal getGravity() const { return gravity; }
    void setGravity(qreal newGravity) { gravity = newGravity; }

    [[nodiscard]] int getDamage() const { return damage; }
    [[nodiscard]] WeaponType getWeaponType() const { return weaponType; }

protected:
    QPointF velocity;
    qreal gravity;
    int damage;
    WeaponType weaponType;
};


#endif //QT_PROGRAMMING_2024_PROJECTILE_H
