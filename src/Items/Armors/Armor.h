//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_ARMOR_H
#define QT_PROGRAMMING_2024_ARMOR_H

#include "../Item.h"
#include "../Mountable.h"
#include "../Weapons/Weapon.h" // 确保包含 Weapon.h，以便 WeaponType 可用

class Armor : public Item, public Mountable {
public:
    explicit Armor(QGraphicsItem *parent, const QString &pixmapPath);

    void mountToParent() override;
    void unmount() override;

    // 护甲系统相关 (来自可选需求)
    virtual int reduceDamage(int incomingDamage, WeaponType weaponType); // 虚函数，允许派生类重写减伤逻辑
    int durability; // 护甲当前耐久度
    int maxDurability; // 护甲最大耐久度
};


#endif //QT_PROGRAMMING_2024_ARMOR_H
