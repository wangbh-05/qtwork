//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include <QDateTime>
#include <QDebug>
#include "Character.h"
#include "../Weapons/FistWeapon.h"
#include "../Maps/Map.h"
#include <QtMath>
#include <QObject>

Character::Character(QGraphicsItem *parent) : Item(parent, ""), // Item 的构造函数会创建 pixmapItem
    health(100), maxHealth(100),
    baseMoveSpeed(0.3), currentMoveSpeed(0.3),
    jumpSpeed(1.0), gravity(0.01),
    hasAdrenalineEffect(false), adrenalineSpeedMultiplier(1.0),
    adrenalineHealPerSecond(0), adrenalineDurationRemaining(0),
    lastAdrenalineHealTime(0),
    lastJumpDown(false),
    punchAnimationTimer(new QTimer(this)),
    debugFrameCounter(0) // 初始化帧计数器
{
    // pixmapItem 已经由 Item 构造函数保证存在，无需在此处手动创建

    defaultWeapon = new FistWeapon(this); // 创建默认武器
    weapon = dynamic_cast<Weapon*>(defaultWeapon); // 装备拳头
    if (weapon) {
        weapon->mountToParent();
    }

    connect(punchAnimationTimer, &QTimer::timeout, this, &Character::revertFromPunchAnimation);

    // defaultIdlePixmap, squatPixmap, walkPixmap, punchPixmap 将在 Link::Link() 中加载和设置
    // 如果没有在 Link 中设置，这里会是空 QPixmap，需要确保使用前检查 .isNull()

    // ====== 初始化生命条 ======
    healthBarBackground = new QGraphicsRectItem(-50, -10, 100, 8, this); // 初始位置相对父项，后续会通过 positionHealthBar 调整
    healthBarBackground->setBrush(Qt::gray);
    healthBarBackground->setZValue(5);

    healthBarFill = new QGraphicsRectItem(-50, -10, 100, 8, this);
    healthBarFill->setBrush(Qt::green);
    healthBarFill->setZValue(6);

    healthTextItem = new QGraphicsTextItem(QString::number(health), this);
    healthTextItem->setDefaultTextColor(Qt::white);
    healthTextItem->setFont(QFont("Arial", 8));
    healthTextItem->setZValue(7);

    // 初始更新生命条显示（内容和颜色），位置会在 Link 构造函数中设置图片后调用 positionHealthBar 调整
    updateHealthDisplay();
}

// QGraphicsItem 虚函数 paint 的实现
void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // Character 的绘制由其 pixmapItem 自动完成
    // 此处可以添加额外的绘制逻辑，如果需要的话
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}


bool Character::isLeftDown() const {
    return leftDown;
}

void Character::setLeftDown(bool leftDown) {
    Character::leftDown = leftDown;
}

bool Character::isRightDown() const {
    return rightDown;
}

void Character::setRightDown(bool rightDown) {
    Character::rightDown = rightDown;
}

bool Character::isPickDown() const {
    return pickDown;
}

void Character::setPickDown(bool pickDown) {
    squatDown = pickDown;
    Character::pickDown = pickDown;
}

const QPointF &Character::getVelocity() const {
    return velocity;
}

void Character::setVelocity(const QPointF &velocity) {
    Character::velocity = velocity;
}

void Character::processInput() {
    // 直接操作成员变量 this->velocity.x，而不再使用局部变量 currentVelocity
    // this->velocity.y 会由 jump() 或 processMovement (重力) 独立管理

    // 【新增调试】打印输入和状态
    /*qDebug() << "INPUT_DEBUG: squatting=" << squatting
             << " isLeftDown=" << isLeftDown()
             << " isRightDown=" << isRightDown()
             << " currentMoveSpeed=" << currentMoveSpeed;
    */
    // 处理下蹲状态的图片切换
    if (squatDown && onGround) {
        squatting = true;
        setVelocity(QPointF(0, velocity.y())); // 下蹲时水平速度为0
        setJumpDown(false); // 下蹲时不能跳跃
        if (pixmapItem && !squatPixmap.isNull()) {
            pixmapItem->setPixmap(squatPixmap); // 显示下蹲图片
        }
    } else {
        bool wasSquatting = squatting; // 记录上一帧是否在下蹲
        squatting = false;

        // 只有当刚结束下蹲，或者当前没有拳头攻击动画在播放时，才根据移动状态更新站立/行走姿态
        if (wasSquatting || !punchAnimationTimer->isActive()) { // 检查拳头动画计时器是否激活
            QPointF currentFrameVelocity = velocity; // 获取当前速度，只修改X
            currentFrameVelocity.setX(0); // 重置水平速度
            if (isLeftDown()) {
                currentFrameVelocity.setX(currentFrameVelocity.x() - currentMoveSpeed);
                //setTransform(QTransform().scale(1, 1)); // 角色朝向右
                if (weapon) weapon->mountToParent();
            } else if (isRightDown()) {
                currentFrameVelocity.setX(currentFrameVelocity.x() + currentMoveSpeed);
                //setTransform(QTransform().scale(-1, 1)); // 角色朝向左
                if (weapon) weapon->mountToParent();
            }
            setVelocity(currentFrameVelocity); // 更新 Item 的速度

            // 根据水平移动状态设置图片
            if (velocity.x() != 0) { // 正在水平移动
                if (pixmapItem && !walkPixmap.isNull()) {
                    pixmapItem->setPixmap(walkPixmap); // 显示行走图片
                } else if (pixmapItem && !defaultIdlePixmap.isNull()) {
                    pixmapItem->setPixmap(defaultIdlePixmap); // 如果没有行走图片，显示待机
                }
            } else { // 静止
                if (pixmapItem && !defaultIdlePixmap.isNull()) {
                    pixmapItem->setPixmap(defaultIdlePixmap); // 显示待机图片
                }
            }
        }
    }


    // 跳跃逻辑：只有当按键刚按下 (isJumpDown && !lastJumpDown) 并且在地面上时才跳跃
    if (isJumpDown() && !lastJumpDown && onGround) {
        jump(); // jump() 会直接修改 this->velocity.y 和 onGround
    }

    if (!lastPickDown && pickDown) {
        picking = true;
    } else {
        picking = false;
    }
    lastPickDown = pickDown;

    // 更新上一次的跳跃键状态
    lastJumpDown = isJumpDown();
}

bool Character::isPicking() const {
    return picking;
}

bool Character::isSquatting() const {
    return squatting;
}

void Character::processMovement(qint64 deltaTime, Map* currentMap) {
    // 更新肾上腺素效果
    updateAdrenalineEffect(deltaTime);

    // 应用重力
    setVelocity(QPointF(velocity.x(), velocity.y() + getGravity() * deltaTime)); // 使用 getGravity()

    setPos(pos() + velocity * (double)deltaTime);

    // 获取当前位置（通常是角色底部中心）对应的地板高度
    qreal charBottomY = pos().y() + boundingRect().height();
    qreal floorHeight = currentMap->getFloorHeightAt(pos() + QPointF(boundingRect().width()/2, boundingRect().height()));

    // 【诊断输出】打印当前物理状态，帮助判断问题
    // 控制打印频率
    debugFrameCounter++;
    if (debugFrameCounter % debugPrintInterval == 0) { // 每隔 debugPrintInterval 帧打印一次
        qDebug() << "FRAME_DEBUG: "
                 << "Char Y:" << pos().y()
                 << " Bottom Y:" << charBottomY
                 << " Floor H:" << floorHeight
                 << " Vel Y:" << velocity.y()
                 << " OnGround:" << onGround;
    }


    // 地板碰撞检测
    // 只有当角色底部接触或穿透地板时才进行处理
    if (charBottomY >= floorHeight) { // 注意这里改为 '>='，更严谨
        setY(floorHeight - boundingRect().height()); // 将角色吸附到地板上沿

        // 关键修正：只有当角色是下落状态（Y速度 >= 0）时，才认为其落地
        if (velocity.y() >= 0) { // 如果是下落（Y速度为正）或静止（Y速度为0）
            setVelocity(QPointF(velocity.x(), 0)); // 停止垂直移动
            onGround = true;  // 标记为在地面上
            // if (debugFrameCounter % debugPrintInterval == 0)
            //     qDebug() << "  -> DEBUG: Landed (vel >= 0)";
        } else {
            // 如果 Y 速度为负（正在向上跳跃），即使暂时接触地板，也保持 onGround = false
            // 让角色继续向上移动，直到重力使其速度变为正值并最终落地
            onGround = false;
            // if (debugFrameCounter % debugPrintInterval == 0)
            //     qDebug() << "  -> DEBUG: Snapped while jumping up (vel < 0)"; // 理论上这里不应该被吸附如果 setY(-1) 有效
        }
    } else {
        // 角色在空中，未接触地板
        onGround = false;
        // if (debugFrameCounter % debugPrintInterval == 0)
        //     qDebug() << "  -> DEBUG: In Air";
    }

    // 应用地形效果
    TerrainType currentTerrain = currentMap->getTerrainTypeAt(pos());
    if (currentTerrain == Ice) {
        currentMoveSpeed = baseMoveSpeed * 1.5 * adrenalineSpeedMultiplier; // 示例：冰面加速 50%
    } else {
        currentMoveSpeed = baseMoveSpeed * adrenalineSpeedMultiplier; // 恢复默认速度或肾上腺素加速
    }

    if (currentTerrain == Grass) {
        if (isSquatting()) {
            setOpacity(0.5);
        } else {
            setOpacity(1.0);
        }
    } else {
        setOpacity(1.0);
    }

    // 场景边界限制
    QRectF sceneRect = scene()->sceneRect();
    if (pos().x() < sceneRect.left()) {
        setX(sceneRect.left());
        setVelocity(QPointF(0, velocity.y())); // 使用 setVelocity
    } else if (pos().x() + boundingRect().width() > sceneRect.right()) {
        setX(sceneRect.right() - boundingRect().width());
        setVelocity(QPointF(0, velocity.y())); // 使用 setVelocity
    }

    // 【新增调试和修正定位】打印图像左右侧和生命条位置
    if (debugFrameCounter % debugPrintInterval == 0) { // 控制打印频率
        if (pixmapItem) { // 确保 pixmapItem 存在
            /*qDebug() << "DEBUG_IMAGE_POS: Image Left X:" << pos().x()
                     << " Image Right X:" << (pos().x() + pixmapItem->pixmap().width()) // 使用 pixmap().width() 确保是实际显示的图片宽度
                     << " Image Width:" << pixmapItem->pixmap().width();
        */
        }

        if (healthBarBackground) {
            /*qDebug() << "DEBUG_HEALTH_BAR_POS: Bar Left X:" << healthBarBackground->pos().x()
            << " Bar Right X:" << (healthBarBackground->pos().x() + healthBarBackground->rect().width())
            << " Bar Top Y:" << healthBarBackground->pos().y();
        */
        }
    }

    // 调用 positionHealthBar 来更新生命条位置
    // 传入 pixmapItem 的实际宽度，如果 pixmapItem 为空或图片未设置，则传入 boundingRect().width() 作为备用
    positionHealthBar(pixmapItem && !pixmapItem->pixmap().isNull() ? pixmapItem->pixmap().width() : boundingRect().width());

}

bool Character::isJumpDown() const {
    return jumpDown;
}

void Character::setJumpDown(bool jumpDown) {
    Character::jumpDown = jumpDown;
}

void Character::jump() {
    setVelocity(QPointF(velocity.x(), -jumpSpeed)); // 使用 setVelocity 设置初始向上速度
    setY(pos().y() - 1);      // 立即将角色向上推1像素，确保它脱离地板
    onGround = false;         // 角色不再在地面上
    // qDbug() << "Character jumped! Initial Vel Y:" << -jumpSpeed; // 已被帧控制，暂时注释
}

Armor *Character::pickupArmor(Armor *newArmor) {
    auto oldArmor = armor;
    if (oldArmor != nullptr) {
        oldArmor->unmount();
        oldArmor->setPos(newArmor->pos());
        oldArmor->setParentItem(parentItem());
    }
    newArmor->setParentItem(this);
    newArmor->mountToParent();
    armor = newArmor;
    qDebug() << "Character picked up new armor.";
    return oldArmor;
}

Weapon *Character::pickupWeapon(Weapon *newWeapon) {
    Weapon *oldWeapon = nullptr;
    if (weapon != defaultWeapon) {
        oldWeapon = weapon;
        if (oldWeapon != nullptr) {
            oldWeapon->unmount();
            oldWeapon->setPos(newWeapon->pos());
            oldWeapon->setParentItem(parentItem()); // 【修正】这里是 oldWeapon, 不是 oldOld
        }
    }

    newWeapon->setParentItem(this);
    newWeapon->mountToParent();
    weapon = newWeapon;
    if (newWeapon->getPixmapItem()) { // 检查 pixmapItem 是否存在
        qDebug() << "Character picked up new weapon:" << newWeapon->getPixmapItem()->pixmap().cacheKey();
    } else {
        qDebug() << "Character picked up new weapon (no pixmap).";
    }
    return oldWeapon;
}

void Character::fireWeapon(const QPointF& targetPos) {
    if (weapon != nullptr) {
        // 只有当角色不处于拳头攻击动画状态时才能开火
        if (!punchAnimationTimer->isActive()) { // 检查拳头动画计时器是否激活
            if (weapon->getType() == FistType) { // 如果是拳头攻击
                if (weapon->use()) {
                    // 触发角色自身的拳头攻击动画
                    if (pixmapItem && !punchPixmap.isNull()) {
                        pixmapItem->setPixmap(punchPixmap); // 显示拳头攻击图片
                    }
                    punchAnimationTimer->start(150); // 拳头动画持续 150ms
                } else {
                    // ... (拳头无用次数) ...
                }
            } else { // 如果是枪械或其他远程武器 (非拳头)
                if (weapon->use()) { // 尝试使用武器
                    QPointF actualTargetPos = targetPos;
                    if (actualTargetPos.isNull()) {
                        qreal targetX = pos().x() + (transform().m11() > 0 ? 1 : -1) * 200;
                        qreal targetY = pos().y();
                        actualTargetPos = QPointF(targetX, targetY);
                    }
                    weapon->attack(this, actualTargetPos); // 武器自己播放动画 (Rifle 会启动自己的动画)

                    // 角色不切换到静态射击姿态，保持当前姿态
                    // 确保角色在发射枪械时保持当前姿态 (由 processInput 决定)
                    // 不需要在这里设置 pixmapItem
                } else {
                    if (weapon->getPixmapItem()) {
                        qDebug() << "Weapon " << weapon->getPixmapItem()->pixmap().cacheKey() << " has no uses left!";
                    } else {
                        qDebug() << "Weapon (no pixmap) has no uses left!";
                    }
                    weapon->unmount();
                    weapon = dynamic_cast<Weapon*>(defaultWeapon);
                    if (weapon) {
                        weapon->mountToParent();
                    }
                }
            }
        } else {
            qDebug() << "Cannot fire: character's punch animation is still playing.";
        }
    } else {
        qDebug() << "Character has no weapon to fire! Equipping default fist.";
        weapon = dynamic_cast<Weapon*>(defaultWeapon);
        if (weapon) {
            weapon->mountToParent();
        }
    }
}

int Character::getHealth() const {
    return health;
}

void Character::takeDamage(int damageAmount, WeaponType weaponType) {
    int finalDamage = damageAmount;
    if (armor != nullptr) {
        finalDamage = armor->reduceDamage(damageAmount, weaponType);
        if (armor->maxDurability > 0) { // 只有有耐久度的护甲才会被破坏
            int armorDamageTaken = qMin(finalDamage, armor->durability);
            armor->durability -= armorDamageTaken;

            if (armor->durability <= 0) {
                qDebug() << "Armor broke!";
                armor->durability = 0;
                Armor* brokenArmor = armor;
                armor = nullptr;
                brokenArmor->unmount();
                if (brokenArmor->scene()) {
                    brokenArmor->scene()->removeItem(brokenArmor);
                    delete brokenArmor;
                }
            }
            finalDamage -= armorDamageTaken;
        }
    }

    health -= finalDamage;
    if (health <= 0) {
        health = 0;
        // TODO: 处理角色死亡 (例如，播放死亡动画，发出死亡信号)
        qDebug() << "Character died!";
    }
    qDebug() << "Character health: " << health << " (took " << finalDamage << " damage)";
    updateHealthDisplay(); // 更新生命条显示
}

void Character::heal(int healAmount) {
    health += healAmount;
    if (health > maxHealth) {
        health = maxHealth;
    }
    qDebug() << "Character healed. Health: " << health;
    updateHealthDisplay(); // 更新生命条显示
}

bool Character::isAlive() const {
    return health > 0;
}

void Character::applyAdrenalineEffect(qreal speedMultiplier, int healPerSec, qint64 durationMs) {
    hasAdrenalineEffect = true;
    adrenalineSpeedMultiplier = speedMultiplier;
    adrenalineHealPerSecond = healPerSec;
    adrenalineDurationRemaining = durationMs;
    lastAdrenalineHealTime = QDateTime::currentMSecsSinceEpoch();

    currentMoveSpeed = baseMoveSpeed * adrenalineSpeedMultiplier;
    qDebug() << "Adrenaline applied! Speed multiplier:" << adrenalineSpeedMultiplier << ", Healing:" << healPerSec << "HP/s, Duration:" << durationMs / 1000 << "s";
}

void Character::updateAdrenalineEffect(qint64 deltaTime) {
    if (hasAdrenalineEffect) {
        adrenalineDurationRemaining -= deltaTime;

        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - lastAdrenalineHealTime >= 1000) {
            int secondsPassed = (currentTime - lastAdrenalineHealTime) / 1000;
            heal(secondsPassed * adrenalineHealPerSecond);
            lastAdrenalineHealTime = currentTime;
        }

        if (adrenalineDurationRemaining <= 0) {
            qDebug() << "Adrenaline effect ended.";
            hasAdrenalineEffect = false;
            adrenalineSpeedMultiplier = 1.0;
            adrenalineHealPerSecond = 0;
            currentMoveSpeed = baseMoveSpeed;
        }
    }
}

// 拳头动画结束后的恢复槽函数
void Character::revertFromPunchAnimation() {
    punchAnimationTimer->stop();
    // 恢复到当前状态 (下蹲/移动/待机)
    if (squatting) {
        if (pixmapItem && !squatPixmap.isNull()) {
            pixmapItem->setPixmap(squatPixmap);
        }
    } else if (velocity.x() != 0) { // 正在水平移动
        if (pixmapItem && !walkPixmap.isNull()) {
            pixmapItem->setPixmap(walkPixmap);
        } else if (pixmapItem && !defaultIdlePixmap.isNull()) {
            pixmapItem->setPixmap(defaultIdlePixmap);
        }
    } else { // 静止
        if (pixmapItem && !defaultIdlePixmap.isNull()) {
            pixmapItem->setPixmap(defaultIdlePixmap);
        }
    }
}

// 更新生命条显示 (新增函数)
void Character::updateHealthDisplay() {
    if (healthBarFill) {
        qreal fillWidth = (qreal)health / maxHealth * healthBarBackground->rect().width();
        healthBarFill->setRect(healthBarBackground->rect().left(), healthBarBackground->rect().top(), fillWidth, healthBarBackground->rect().height());
        // 根据生命值调整颜色
        if ((qreal)health / maxHealth < 0.3) {
            healthBarFill->setBrush(Qt::red);
        } else if ((qreal)health / maxHealth < 0.6) {
            healthBarFill->setBrush(Qt::yellow);
        } else {
            healthBarFill->setBrush(Qt::green);
        }
    }
    if (healthTextItem) {
        healthTextItem->setPlainText(QString::number(health));
    }
}

// 根据角色视觉宽度定位生命条
void Character::positionHealthBar(qreal characterVisualWidth) {
    if (healthBarBackground && healthBarFill && healthTextItem && pixmapItem) {
        // 计算生命条和文字相对于角色图片中心的偏移
        qreal barWidth = healthBarBackground->rect().width();
        qreal barXOffset = (characterVisualWidth / 2.0) - (barWidth / 2.0); // 居中偏移

        // 生命条 Y 坐标 (相对角色图片顶部的固定偏移)
        qreal barYOffset = -30; // 假设生命条在角色图片顶部上方 30 像素

        healthBarBackground->setPos(barXOffset, barYOffset);
        healthBarFill->setPos(barXOffset, barYOffset);

        // 文字 X 坐标 (相对生命条中心的偏移)
        qreal textWidth = healthTextItem->boundingRect().width();
        healthTextItem->setPos(barXOffset + barWidth / 2 - textWidth / 2, barYOffset - 15); // 文字在生命条上方居中
    }
}
