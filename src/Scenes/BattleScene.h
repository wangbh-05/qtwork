//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"
#include "../Items/Mountable.h"
#include <QList>
#include "../Items/Consumables/Consumable.h" // 包含 Consumable.h
#include "../Items/Weapons/Weapon.h" // 包含 Weapon.h 以识别 WeaponType
#include "../Items/Projectiles/Projectile.h" // 包含 Projectile.h
#include "../Items/Item.h" // 确保包含 Item.h 以便使用 Item*
#include <QTimer> // 【新增】 包含 QTimer 头文件

class BattleScene : public Scene {
    Q_OBJECT

public:
    explicit BattleScene(QObject *parent);

    void processInput() override;
    void processMovement() override;
    void processPicking() override;
    void processProjectiles();

protected slots:
    void update() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // 修改：findNearestUnmountedMountable 的返回类型和参数类型调整为 Item*，因为我们实际操作的是 QGraphicsItem 的子类
    Item *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());
    // 修改：pickupMountable 的参数类型为 QGraphicsItem*
    Mountable *pickupMountable(Character *character, QGraphicsItem *itemToPickup);

    Map *map;
    Character *character;
    Armor *spareArmor; // 这是一个注释掉的成员，根据需要可以移除
    // TODO: 添加 AI 角色: AICharacter *aiCharacter;

    QTimer* itemSpawnTimer; // 【新增】物品生成计时器声明
    void spawnRandomItem(); // 【新增】随机生成物品的函数声明
};


#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
