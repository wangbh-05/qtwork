//
// Created by gerw on 8/20/24.
//

#include <QDebug>
#include <QCursor>
#include <QList>
#include <QRandomGenerator> // 用于生成随机数
#include "BattleScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/FlamebreakerArmor.h" // 仍包含，但 spareArmor 不会初始化
#include "../Items/Weapons/Rifle.h"
#include "../Items/Weapons/FistWeapon.h"
#include "../Items/Projectiles/Projectile.h"
#include "../Items/Consumables/Bandage.h"
#include "../Items/Consumables/Medkit.h"
#include "../Items/Consumables/Adrenaline.h"
#include "../Items/Armors/OldShirt.h" // 确保包含 OldShirt 以供随机生成
#include "../Items/Item.h" // 确保包含 Item.h

BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 768); // 假设你希望的场景尺寸是 1280x768

    map = new Battlefield();
    character = new Link();


    addItem(map);
    addItem(character);

    // 示例掉落物品 (现在由 spawnRandomItem() 处理，这些可以移除或留作初始测试)
    // Rifle* spareRifle = new Rifle();
    // Bandage* spareBandage = new Bandage();
    // Medkit* spareMedkit = new Medkit();
    // Adrenaline* spareAdrenaline = new Adrenaline();


    // addItem(spareArmor); // 用户要求注释掉护甲的初始化
    // addItem(spareRifle); // 现在由 spawnRandomItem() 处理
    // addItem(spareBandage);
    // addItem(spareMedkit);
    // addItem(spareAdrenaline);


    map->scaleToFitScene(this); // 这行代码会根据新的 setSceneRect 调整地图大小
    character->setPos(map->getSpawnPos());

    // 修复：将 sceneRect() 的返回值存储到局部变量中，避免重复调用和语法错误
    // QRectF currentSceneRect = sceneRect(); // 在 BattleScene 内部方法可以直接访问 sceneRect()

    // 初始化物品生成计时器并启动
    itemSpawnTimer = new QTimer(this);
    connect(itemSpawnTimer, &QTimer::timeout, this, &BattleScene::spawnRandomItem);
    itemSpawnTimer->start(3000); // 每 3 秒生成一个物品 (可以调整频率，例如 3000ms = 3秒)

    // 之前手动设置物品位置的代码现在可以删除，因为有了随机生成器
    // spareRifle->unmount();
    // spareRifle->setPos(currentSceneRect.left() + (currentSceneRect.right() - currentSceneRect.left()) * 0.25, map->getFloorHeightAt(spareRifle->pos()) - spareRifle->boundingRect().height());
    // qDebug() << "BattleScene: Rifle placed at initial position:" << spareRifle->pos();

    // spareBandage->setPos(currentSceneRect.left() + (currentSceneRect.right() - currentSceneRect.left()) * 0.5, map->getFloorHeightAt(spareBandage->pos()) - spareBandage->boundingRect().height());
    // qDebug() << "BattleScene: Bandage placed at initial position:" << spareBandage->pos();

    // spareMedkit->setPos(currentSceneRect.left() + (currentSceneRect.right() - currentSceneRect.left()) * 0.5, map->getFloorHeightAt(spareMedkit->pos()) - spareMedkit->boundingRect().height());
    // qDebug() << "BattleScene: Medkit placed at initial position:" << spareMedkit->pos();

    // spareAdrenaline->setPos(currentSceneRect.left() + (currentSceneRect.right() - currentSceneRect.left()) * 0.5, map->getFloorHeightAt(spareAdrenaline->pos()) - spareAdrenaline->boundingRect().height());
    // qDebug() << "BattleScene: Adrenaline placed at initial position:" << spareAdrenaline->pos();
}

void BattleScene::processInput() {
    Scene::processInput();
    if (character != nullptr) {
        character->processInput();
    }
}

void BattleScene::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A:
        if (character != nullptr) {
            character->setLeftDown(true);
        }
        break;
    case Qt::Key_D:
        if (character != nullptr) {
            character->setRightDown(true);
        }
        break;
    case Qt::Key_J: // 拾取键
        if (character != nullptr) {
            character->setPickDown(true);
            //qDebug() << "KeyPress: J (PickDown) set to TRUE.";
        }
        break;
    case Qt::Key_W:
        if (character != nullptr) {
            character->setJumpDown(true);
        }
        break;
    case Qt::Key_K: // 射击键
        if (character != nullptr) {
            character->fireWeapon(character->pos() + character->boundingRect().center()); // 用于测试的自瞄准
        }
        break;
    default:
        Scene::keyPressEvent(event);
    }
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A:
        if (character != nullptr) {
            character->setLeftDown(false);
        }
        break;
    case Qt::Key_D:
        if (character != nullptr) {
            character->setRightDown(false);
        }
        break;
    case Qt::Key_J: // 拾取键
        if (character != nullptr) {
            character->setPickDown(false);
            //qDebug() << "KeyRelease: J (PickDown) set to FALSE.";
        }
        break;
    case Qt::Key_W:
        if (character != nullptr) {
            character->setJumpDown(false);
        }
        break;
    case Qt::Key_K:
        break;
    default:
        Scene::keyReleaseEvent(event);
    }
}

void BattleScene::update() {
    Scene::update();
    processProjectiles();
}

void BattleScene::processMovement() {
    Scene::processMovement();
    if (character != nullptr) {
        character->processMovement(deltaTime, map);
    }

    // 处理掉落物品的重力
    QList<QGraphicsItem*> itemsToRemove;
    for (QGraphicsItem* item : items()) {
        // 检查 item 是否是 Item 类型
        if (auto movableItem = dynamic_cast<Item*>(item)) {
            // 【核心修正】排除地图对象，不对其施加重力
            if (movableItem == map) continue;

            // 排除角色、弹丸 (它们有各自的 processMovement)
            // 排除已装备的物品
            if (dynamic_cast<Character*>(movableItem) == nullptr &&
                dynamic_cast<Projectile*>(movableItem) == nullptr) {

                bool isMounted = false;
                if (auto mountable = dynamic_cast<Mountable*>(movableItem)) {
                    isMounted = mountable->isMounted();
                }

                if (!isMounted) { // 只对掉落的、未装备的物品施加重力
                    // 应用重力
                    QPointF currentVel = movableItem->getVelocity();
                    currentVel.setY(currentVel.y() + character->getGravity() * deltaTime);
                    movableItem->setVelocity(currentVel);

                    movableItem->setPos(movableItem->pos() + currentVel * (double)deltaTime);

                    // 地板碰撞检测
                    qreal itemFloorHeight = map->getFloorHeightAt(movableItem->pos() + QPointF(movableItem->boundingRect().width()/2, movableItem->boundingRect().height()));
                    if (movableItem->pos().y() + movableItem->boundingRect().height() >= itemFloorHeight) {
                        movableItem->setY(itemFloorHeight - movableItem->boundingRect().height());
                        movableItem->setVelocity(QPointF(0,0)); // 停止垂直移动
                    }

                    // 物品出界移除 (如果掉出屏幕底部)
                    if (movableItem->pos().y() > sceneRect().height() + 50) {
                        itemsToRemove.append(movableItem);
                        qDebug() << "Dropped item " << (dynamic_cast<QObject*>(movableItem) ? dynamic_cast<QObject*>(movableItem)->metaObject()->className() : "Non-QObject Item") << " removed (fell off screen).";
                    }
                }
            }
        }
    }
    // 移除并删除掉落物品
    for (QGraphicsItem* item : itemsToRemove) {
        removeItem(item);
        delete item;
    }
}

// 【新增】随机生成物品的函数实现
void BattleScene::spawnRandomItem() {
    // 定义可以生成的物品类型
    enum SpawnableItemType {
        SpawnRifle,
        SpawnBandage,
        SpawnMedkit,
        SpawnAdrenaline,
        SpawnOldShirt, // 护甲也作为掉落物
        NumSpawnableItemTypes // 总类型数量
    };

    int randomType = QRandomGenerator::global()->bounded(NumSpawnableItemTypes); // 随机生成物品类型索引
    Item* newItem = nullptr;

    switch (randomType) {
    case SpawnRifle: newItem = new Rifle(); break;
    case SpawnBandage: newItem = new Bandage(); break;
    case SpawnMedkit: newItem = new Medkit(); break;
    case SpawnAdrenaline: newItem = new Adrenaline(); break;
    //case SpawnOldShirt: newItem = new OldShirt(); break;
    default: return; // 理论上不会发生
    }

    if (newItem) {
        // 随机 X 坐标，确保物品完全在场景宽度内
        qreal randomX = QRandomGenerator::global()->bounded( (int)(sceneRect().width() - newItem->boundingRect().width()) );
        if (randomX < 0) randomX = 0; // 防止负值

        qreal spawnY = sceneRect().top() - newItem->boundingRect().height(); // 在场景顶部生成，使其完全可见开始下落

        newItem->setPos(randomX, spawnY);
        addItem(newItem);
        qDebug() << "Spawned random item: " << (dynamic_cast<QObject*>(newItem) ? dynamic_cast<QObject*>(newItem)->metaObject()->className() : "Non-QObject Item") << " at " << newItem->pos();
    }
}


void BattleScene::processPicking() {
    Scene::processPicking(); // 调用 Character::isPicking() 等
    //qDebug() << "ProcessPicking: isPicking =" << (character ? character->isPicking() : false);
    if (character && character->isPicking()) { // 确保 character 不为 nullptr
        Item *nearestItemToPickup = findNearestUnmountedMountable(character->pos(), 500.);
        if (nearestItemToPickup != nullptr) {
            qDebug() << "ProcessPicking: Found nearest item at" << nearestItemToPickup->pos() << "Class:" << (dynamic_cast<QObject*>(nearestItemToPickup) ? dynamic_cast<QObject*>(nearestItemToPickup)->metaObject()->className() : "Non-QObject Item") << ". Its mounted state: " << (dynamic_cast<Mountable*>(nearestItemToPickup) ? dynamic_cast<Mountable*>(nearestItemToPickup)->isMounted() : false);
            Mountable* droppedOldItem = pickupMountable(character, nearestItemToPickup);
            if (droppedOldItem) { // 只有当有旧物品被丢弃时才处理
                if (QGraphicsItem* droppedGraphicsItem = dynamic_cast<QGraphicsItem*>(droppedOldItem)) {
                    if (droppedGraphicsItem->scene() == nullptr) { // 检查是否已在场景中
                        addItem(droppedGraphicsItem); // addItem 期望 QGraphicsItem*
                        qDebug() << "ProcessPicking: Added dropped old item to scene.";
                    }
                } else {
                    qDebug() << "ProcessPicking: Warning: droppedOldItem could not be cast to QGraphicsItem*. Memory leak possible.";
                }
            }
        } else {
            qDebug() << "ProcessPicking: No unmounted mountable item found in range.";
        }
    }
}

Item *BattleScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold) {
    Item *nearest = nullptr;
    qreal minDistance = distance_threshold;
    qDebug() << "findNearestUnmountedMountable: Searching for items near" << pos << "with threshold" << distance_threshold;

    for (QGraphicsItem *item: items()) {
        if (item == character) continue; // 跳过角色自身
        // TODO: 如果有 AICharacter，也跳过它

        if (auto currentItemAsItem = dynamic_cast<Item *>(item)) {
            // 检查它是否是 Mountable (武器或护甲)
            if (auto mountable = dynamic_cast<Mountable *>(currentItemAsItem)) {
                if (!mountable->isMounted()) { // 只查找未装备的物品
                    qreal distance = QLineF(pos, currentItemAsItem->pos()).length();
                    qDebug() << "  - Checking item at" << currentItemAsItem->pos() << "Class:" << (dynamic_cast<QObject*>(currentItemAsItem) ? dynamic_cast<QObject*>(currentItemAsItem)->metaObject()->className() : "Non-QObject Item") << "Distance:" << distance << "Mounted:" << mountable->isMounted();
                    if (distance < minDistance) {
                        minDistance = distance;
                        nearest = currentItemAsItem;
                        qDebug() << "    - Found closer item:" << (dynamic_cast<QObject*>(nearest) ? dynamic_cast<QObject*>(nearest)->metaObject()->className() : "Non-QObject Item");
                    }
                }
            }
            // 检查它是否是 Consumable
            else if (dynamic_cast<Consumable*>(currentItemAsItem)) { // 消耗品是 Item 但不是 Mountable
                qreal distance = QLineF(pos, currentItemAsItem->pos()).length();
                qDebug() << "  - Checking Consumable item at" << currentItemAsItem->pos() << "Class:" << (dynamic_cast<QObject*>(currentItemAsItem) ? dynamic_cast<QObject*>(currentItemAsItem)->metaObject()->className() : "Non-QObject Item") << "Distance:" << distance;
                if (distance < minDistance) {
                    minDistance = distance;
                    nearest = currentItemAsItem;
                    qDebug() << "    - Found closer Consumable:" << (dynamic_cast<QObject*>(nearest) ? dynamic_cast<QObject*>(nearest)->metaObject()->className() : "Non-QObject Item");
                }
            }
        }
    }
    qDebug() << "findNearestUnmountedMountable: Nearest found (final):" << (nearest ? (dynamic_cast<QObject*>(nearest) ? dynamic_cast<QObject*>(nearest)->metaObject()->className() : "Non-QObject Item") : "None");
    return nearest;
}

Mountable *BattleScene::pickupMountable(Character *character, QGraphicsItem *itemToPickup) {
    Mountable* oldItem = nullptr;
    qDebug() << "pickupMountable: Attempting to pick up item of class:" << (dynamic_cast<QObject*>(itemToPickup) ? dynamic_cast<QObject*>(itemToPickup)->metaObject()->className() : "Non-QObject Item");

    // 首先尝试转换为 Mountable (武器或护甲)
    if (auto mountable = dynamic_cast<Mountable *>(itemToPickup)) {
        qDebug() << "  - Item is Mountable.";
        if (auto armor = dynamic_cast<Armor *>(mountable)) {
            oldItem = character->pickupArmor(armor);
            qDebug() << "    - Item identified as Armor. Old armor returned:" << (oldItem ? (dynamic_cast<QObject*>(oldItem) ? dynamic_cast<QObject*>(oldItem)->metaObject()->className() : "Non-QObject Item") : "None");
        } else if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
            oldItem = character->pickupWeapon(weapon);
            qDebug() << "    - Item identified as Weapon. Old weapon returned:" << (oldItem ? (dynamic_cast<QObject*>(oldItem) ? dynamic_cast<QObject*>(oldItem)->metaObject()->className() : "Non-QObject Item") : "None");
        }
    }
    // 接着尝试转换为 Consumable (如果不是 Mountable)
    else if (auto consumable = dynamic_cast<Consumable *>(itemToPickup)) {
        qDebug() << "  - Item is Consumable. Applying effect...";
        consumable->applyEffect(character);
        if (itemToPickup->scene()) {
            removeItem(itemToPickup);
            delete itemToPickup;
            qDebug() << "    - Consumable removed from scene and deleted.";
        }
        return nullptr; // Consumables are consumed, no old item to return
    } else {
        qDebug() << "  - Item is neither Mountable nor Consumable. Cannot pick up.";
    }

    // 对于非消耗品，如果被成功拾取 (oldItem 不为 nullptr 或没有 oldItem 返回但物品成功装备)，就从场景中移除
    if (itemToPickup && itemToPickup->scene()) { // 确保 itemToPickup 仍然有效且在场景中
        // 只有当它不是 Consumable 时才移除 (Consumable 已经在上面逻辑中移除了)
        if (!dynamic_cast<Consumable*>(itemToPickup)) {
            removeItem(itemToPickup);
            qDebug() << "pickupMountable: Item removed from scene (non-consumable, not consumed).";
        }
    } else {
        qDebug() << "pickupMountable: Item not removed from scene (already removed, invalid, or consumable).";
    }

    return oldItem;
}

void BattleScene::processProjectiles() {
    QList<QGraphicsItem*> itemsToRemove;

    // 修复：确保在这里获取 sceneRect() 的值，而不是每次都调用
    QRectF currentSceneRect = sceneRect();

    for (QGraphicsItem* item : items()) {
        if (auto projectile = dynamic_cast<Projectile*>(item)) {
            projectile->processMovement(deltaTime);

            // 弹丸与角色碰撞检测
            if (character != nullptr && projectile->collidesWithItem(character)) {
                // 确保角色存活才受击
                if (character->isAlive()) {
                    character->takeDamage(projectile->getDamage(), projectile->getWeaponType());
                }
                itemsToRemove.append(projectile);
            }
            // TODO: 如果有 AICharacter，也进行碰撞检测

            // 弹丸出界检查 (简单示例)
            if (!currentSceneRect.contains(projectile->pos())) { // 使用局部变量 currentSceneRect
                itemsToRemove.append(projectile);
            }
        }
    }

    for (QGraphicsItem* item : itemsToRemove) {
        removeItem(item);
        delete item;
    }
}
