//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QTimer>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include "../HeadEquipments/HeadEquipment.h"
#include "../Armors/Armor.h"
#include "../LegEquipments/LegEquipment.h"
#include "../Weapons/Weapon.h"
#include "../Maps/Map.h"
#include "../Item.h"

class Character : public Item { // 继承 Item
    Q_OBJECT

public:
    explicit Character(QGraphicsItem *parent);

    // QGraphicsItem 虚函数 (由 Item 基类提供默认实现或由 Character 覆盖)
    // boundingRect 和 getPixmapItem() 已在 Item.h 中声明并实现，这里无需重复声明
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;


    [[nodiscard]] bool isLeftDown() const;
    void setLeftDown(bool leftDown);

    [[nodiscard]] bool isRightDown() const;
    void setRightDown(bool rightDown);

    [[nodiscard]] bool isPickDown() const;
    void setPickDown(bool pickDown);

    [[nodiscard]] bool isJumpDown() const;
    void setJumpDown(bool jumpDown);
    void jump();

    [[nodiscard]] const QPointF &getVelocity() const;
    void setVelocity(const QPointF &velocity);

    [[nodiscard]] bool isPicking() const;
    [[nodiscard]] bool isSquatting() const;

    void processInput();
    virtual void processMovement(qint64 deltaTime, Map* currentMap);

    Armor* pickupArmor(Armor* newArmor);
    Weapon* pickupWeapon(Weapon* newWeapon);
    void fireWeapon(const QPointF& targetPos = QPointF());

    // 生命值系统相关
    [[nodiscard]] int getHealth() const;
    [[nodiscard]] int getMaxHealth() const { return maxHealth; }
    void takeDamage(int damageAmount, WeaponType weaponType = OtherWeaponType);
    void heal(int healAmount);
    [[nodiscard]] bool isAlive() const;

    // 肾上腺素效果相关
    void applyAdrenalineEffect(qreal speedMultiplier, int healPerSec, qint64 durationMs);
    void updateAdrenalineEffect(qint64 deltaTime);

public slots:
    void revertFromPunchAnimation(); // 拳头动画结束后的恢复槽函数

public: // getGravity()改为public访问权限
    [[nodiscard]] qreal getGravity() const { return gravity; } // Character 独有的重力值


protected:
    // 【核心修正】移除成员变量声明前的错误命名空间限定符
    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
    Weapon *weapon{};

    QPointF velocity{};

    // 移动/输入状态
    bool leftDown{}, rightDown{}, pickDown{};
    bool lastPickDown{};
    bool picking{};
    bool jumpDown{};
    bool squatDown{};
    bool squatting{};
    bool onGround{false};

    qreal baseMoveSpeed;
    qreal currentMoveSpeed;
    qreal jumpSpeed;
    qreal gravity; // Character 独有的重力值


    // 生命值系统相关
    int health;
    int maxHealth;

    // 视觉生命条相关 (新增)
    QGraphicsRectItem* healthBarBackground{};
    QGraphicsRectItem* healthBarFill{};
    QGraphicsTextItem* healthTextItem{};
    void updateHealthDisplay();
    void positionHealthBar(qreal characterVisualWidth);

    // 肾上腺素效果相关
    bool hasAdrenalineEffect;
    qreal adrenalineSpeedMultiplier;
    int adrenalineHealPerSecond;
    qint64 adrenalineDurationRemaining;
    qint64 lastAdrenalineHealTime;

    // 动画相关成员 (简化后)
    QPixmap defaultIdlePixmap;
    QPixmap squatPixmap;
    QPixmap walkPixmap;
    QPixmap punchPixmap;
    QTimer* punchAnimationTimer;

    // Debug 打印相关 (新增)
    int debugFrameCounter;
    const int debugPrintInterval = 900; // 每隔 60 帧打印一次

private:
    Mountable *defaultWeapon;
    bool lastJumpDown{};
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
