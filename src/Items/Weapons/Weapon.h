//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_WEAPON_H
#define QT_PROGRAMMING_2024_WEAPON_H

#include "../Item.h"
#include "../Mountable.h"
// #include "../Characters/Character.h" // 这里的 Character.h 应该在 .cpp 中包含，此处仅用前向声明

class Character; // 前向声明 Character

// 在这里定义 WeaponType 枚举
enum WeaponType {
    FistType,
    KnifeType,
    BulletType, // 用于步枪/狙击枪的子弹类型
    SolidBallType,
    OtherWeaponType
};

class Weapon : public Item, public Mountable {
public:
    explicit Weapon(QGraphicsItem *parent, const QString &pixmapPath);

    // 纯虚函数，所有派生武器必须实现其攻击逻辑
    virtual void attack(Character *attacker, const QPointF& targetPos) = 0;

    // 重写 Mountable 的方法，用于武器特定的装备位置和缩放
    void mountToParent() override;
    void unmount() override;

    [[nodiscard]] int getDamage() const { return damage; }
    [[nodiscard]] int getRemainingUses() const { return remainingUses; }
    [[nodiscard]] qreal getAttackRange() const { return attackRange; }
    [[nodiscard]] qint64 getAttackInterval() const { return attackInterval; }
    [[nodiscard]] qint64 getLastAttackTime() const { return lastAttackTime; }
    [[nodiscard]] WeaponType getType() const { return type; }

    // 减少武器使用次数，返回是否可以成功使用（已考虑冷却和次数）
    bool use();

protected:
    int damage; // 武器造成的伤害
    int remainingUses; // 剩余使用次数，-1 表示无限次
    qreal attackRange; // 攻击范围
    qint64 attackInterval; // 攻击间隔（毫秒），即冷却时间
    qint64 lastAttackTime; // 上次攻击的时间戳（毫秒）
    WeaponType type; // 武器类型

    // 用于在攻击时创建弹丸等效果的辅助函数
    // attacker 必须是完整类型，因此在 .cpp 中包含其头文件
    void createProjectile(Character *attacker, const QPointF& targetPos, qreal speed, qreal gravity = 0, const QString& pixmap = "");
};


#endif //QT_PROGRAMMING_2024_WEAPON_H
