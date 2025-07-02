//
// Created by gerw on 8/20/24.
//

#include <memory>
#include "Link.h"
#include "../HeadEquipments/CapOfTheHero.h"
#include "../Armors/OldShirt.h"
#include "../LegEquipments/WellWornTrousers.h"
#include <QPixmap> // 确保包含 QPixmap 头文件
#include <QDebug> // 用于调试输出

Link::Link(QGraphicsItem *parent) : Character(parent) {
    // Character 构造函数已经初始化了默认武器 (FistWeapon)
    // 并且也已经创建并启动了 punchAnimationTimer，连接了 revertFromPunchAnimation
    // 所以这里只需初始化其他装备和加载 Link 特有的静态图片/动画帧

    // 注释掉 HeadEquipment 和 LegEquipment 的创建和挂载 (保持你自行注释的部分)
    // headEquipment = new CapOfTheHero(this);
    // legEquipment = new WellWornTrousers(this);
    //armor = new OldShirt(this); // 旧衬衫作为初始护甲
    // headEquipment->mountToParent(); // 注释掉
    // legEquipment->mountToParent(); // 注释掉
    //armor->mountToParent();

    // ====== Link 角色图片加载与缩放 ======

    // 定义统一的目标宽度和缩放模式
    const int targetWidth = 128; // 目标宽度为 256 像素
    const Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio; // 保持宽高比
    const Qt::TransformationMode transformMode = Qt::SmoothTransformation; // 平滑缩放（质量更高）

    // 默认站立姿态
    QPixmap loadedNormalPixmap(":/Items/Characters/Link/link_normal.png"); // 加载原始图片文件
    if (loadedNormalPixmap.isNull()) {
        qDebug() << "Error: Could not load link_normal.png!";
        defaultIdlePixmap = QPixmap(targetWidth, targetWidth); defaultIdlePixmap.fill(Qt::blue); // 占位符，正方形
    } else {
        // 【核心修正】缩放图片，保持宽高比，宽度为 targetWidth
        defaultIdlePixmap = loadedNormalPixmap.scaled(targetWidth, loadedNormalPixmap.height(), aspectRatioMode, transformMode);
        qDebug() << "Link defaultIdlePixmap scaled to:" << defaultIdlePixmap.size();
    }

    if (pixmapItem) { // 确保 pixmapItem 不为空
        pixmapItem->setPixmap(defaultIdlePixmap); // 设置初始图片为站立姿态
        qDebug() << "Link's initial pixmap set to link_normal.png (scaled).";
    } else {
        qDebug() << "Warning: Link's pixmapItem is null in Link constructor, cannot set initial pixmap!";
    }


    // 下蹲姿态
    QPixmap loadedSquatPixmap(":/Items/Characters/Link/link_squat.png"); // 加载原始图片
    if (loadedSquatPixmap.isNull()) {
        qDebug() << "Error: Could not load link_squat.png!";
        squatPixmap = QPixmap(targetWidth, targetWidth); squatPixmap.fill(Qt::darkBlue); // 占位符
    } else {
        // 【核心修正】缩放图片
        squatPixmap = loadedSquatPixmap.scaled(targetWidth, loadedSquatPixmap.height(), aspectRatioMode, transformMode);
        qDebug() << "Link squatPixmap scaled to:" << squatPixmap.size();
    }

    // 行走姿态 (如果 Link 有单独的行走图片)
    QPixmap loadedWalkPixmap(":/Items/Characters/Link/link_walk_1.png"); // 加载原始图片
    if (loadedWalkPixmap.isNull()) {
        qDebug() << "Warning: Could not load link_walk_1.png. Walk will use idle image.";
        walkPixmap = defaultIdlePixmap; // 如果没有行走图片，则行走时也显示站立图片
    } else {
        // 【核心修正】缩放图片
        walkPixmap = loadedWalkPixmap.scaled(targetWidth, loadedWalkPixmap.height(), aspectRatioMode, transformMode);
        qDebug() << "Link walkPixmap scaled to:" << walkPixmap.size();
    }

    // 拳头攻击姿态
    QPixmap loadedPunchPixmap(":/Items/Characters/Link/link_punch.png"); // 加载原始图片
    if (loadedPunchPixmap.isNull()) {
        qDebug() << "Error: Could not load link_punch.png!";
        punchPixmap = defaultIdlePixmap; // 如果没有拳头攻击图片，则用待机图片代替
    } else {
        // 【核心修正】缩放图片
        punchPixmap = loadedPunchPixmap.scaled(targetWidth, loadedPunchPixmap.height(), aspectRatioMode, transformMode);
        qDebug() << "Link punchPixmap scaled to:" << punchPixmap.size();
    }
}
