//
// Created by Gemini on 2025/06/27.
//

#include "Rifle.h"
#include <QDebug>
#include <QPixmap>
#include <QTimer>
#include "../Characters/Character.h" // 确保包含 Character.h 的完整定义

Rifle::Rifle(QGraphicsItem *parent)
    : Weapon(parent, ""), // 先不指定 pixmapPath，Item 基类会创建 pixmapItem
    animationTimer(nullptr), // 在构造函数中初始化为 nullptr
    currentAnimationFrameIndex(0),
    frameDurationMs(50) // 每帧 50ms，动画会比较快
{
    damage = 20;
    remainingUses = 30;
    attackRange = 1000;
    type = BulletType;

    // 定义 Rifle 图片的统一目标宽度和缩放模式
    const int rifleTargetWidth = 256; // 目标宽度为 96 像素（你可以根据需要调整，例如 64 或 128）
    const Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio; // 保持宽高比
    const Qt::TransformationMode transformMode = Qt::SmoothTransformation; // 平滑缩放（质量更高）

    // ====== 从精灵图中加载和提取动画帧 ======
    qDebug() << "Rifle Constructor: Attempting to load spritesheet...";
    QPixmap rifleSpritesheet(":/Items/Weapons/Rifle/rifle_attack_spritesheet.png"); // TODO: 替换为你的步枪攻击精灵图路径
    if (rifleSpritesheet.isNull()) {
        qDebug() << "Error: Could not load rifle attack spritesheet! Path: :/Items/Weapons/Rifle/rifle_attack_spritesheet.png";
        defaultPixmap = QPixmap(rifleTargetWidth, rifleTargetWidth / 2); defaultPixmap.fill(Qt::magenta); // 占位符
    } else {
        int spritesheetOriginalFrameWidth = 96 / 12; // 原始单帧宽度 96 / 12 = 8 像素
        int spritesheetOriginalFrameHeight = 48; // 原始单帧高度 48 像素

        qDebug() << "Rifle Spritesheet Loaded. Original Frame Dims: " << spritesheetOriginalFrameWidth << "x" << spritesheetOriginalFrameHeight;

        attackAnimationFrames.clear();
        for (int i = 0; i < 12; ++i) { // 12 帧在同一行
            QRect frameRect(i * spritesheetOriginalFrameWidth, 0, spritesheetOriginalFrameWidth, spritesheetOriginalFrameHeight);
            // 【核心修正】提取帧后立即缩放
            attackAnimationFrames.append(rifleSpritesheet.copy(frameRect).scaled(rifleTargetWidth, spritesheetOriginalFrameHeight, aspectRatioMode, transformMode));
        }

        if (!attackAnimationFrames.isEmpty()) {
            defaultPixmap = attackAnimationFrames[0]; // 第一帧（已缩放）作为默认图片
            qDebug() << "Rifle defaultPixmap set from attackAnimationFrames[0]. Size:" << defaultPixmap.size();
        } else {
            defaultPixmap = QPixmap(rifleTargetWidth, rifleTargetWidth / 2); defaultPixmap.fill(Qt::magenta);
            qDebug() << "Rifle defaultPixmap set to placeholder (frames empty).";
        }
    }

    // 设置步枪的初始图片
    if (pixmapItem) {
        pixmapItem->setPixmap(defaultPixmap);
        qDebug() << "Rifle pixmapItem set with defaultPixmap (from spritesheet). Final pixmap size:" << pixmapItem->pixmap().size();
    } else {
        qDebug() << "Rifle constructor: ERROR - pixmapItem is NULL and cannot set defaultPixmap!";
    }

    // 初始化计时器并连接槽函数
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Rifle::advanceAnimationFrame);
}

void Rifle::attack(Character *attacker, const QPointF& targetPos) {
    qDebug() << "Rifle Attack: Called by character at" << attacker->pos() << "towards" << targetPos;

    createProjectile(attacker, targetPos, 1.0, 0, ":/Items/Projectiles/bullet.png");

    // 启动步枪的攻击动画
    if (!attackAnimationFrames.isEmpty() && animationTimer && !animationTimer->isActive()) {
        currentAnimationFrameIndex = 0;
        if (pixmapItem) pixmapItem->setPixmap(attackAnimationFrames[currentAnimationFrameIndex]); // 显示动画第一帧
        animationTimer->start(frameDurationMs); // 启动计时器
        qDebug() << "Rifle Animation: Started.";
    } else {
        qDebug() << "Rifle Animation: Cannot start. Frames empty or timer active.";
    }
}

// 槽函数：推进动画帧
void Rifle::advanceAnimationFrame() {
    currentAnimationFrameIndex++;
    if (currentAnimationFrameIndex >= attackAnimationFrames.size()) {
        // 动画播放完毕
        if (animationTimer) animationTimer->stop(); // 停止计时器
        currentAnimationFrameIndex = 0; // 重置帧索引
        if (pixmapItem) pixmapItem->setPixmap(defaultPixmap); // 恢复步枪的默认姿态
        qDebug() << "Rifle Animation: Finished, restored default pixmap.";
    } else {
        if (pixmapItem) pixmapItem->setPixmap(attackAnimationFrames[currentAnimationFrameIndex]); // 显示下一帧
        qDebug() << "Rifle Animation: Showing frame" << currentAnimationFrameIndex + 1;
    }
}
