//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_RIFLE_H
#define QT_PROGRAMMING_2024_RIFLE_H

#include "Weapon.h"
#include <QVector> // 新增：包含 QVector
#include <QPixmap> // 新增：包含 QPixmap
#include <QTimer>  // 新增：包含 QTimer

class Rifle : public Weapon {
    Q_OBJECT // 确保 Q_OBJECT 宏存在，因为它使类能够使用信号槽机制

public:
    explicit Rifle(QGraphicsItem *parent = nullptr); // 允许无父项构造，以便掉落
    void attack(Character *attacker, const QPointF& targetPos) override;

public slots: // 新增：槽函数，可以连接到信号
    void advanceAnimationFrame(); // 动画帧切换槽函数

protected:
    QVector<QPixmap> attackAnimationFrames; // 存储步枪攻击动画的帧
    QTimer* animationTimer;               // 控制步枪动画的计时器
    int currentAnimationFrameIndex;       // 当前播放帧的索引
    qint64 frameDurationMs;                // 每帧持续时间 (毫秒)
    QPixmap defaultPixmap;                // 步枪的默认（非攻击）姿态图片
};


#endif //QT_PROGRAMMING_2024_RIFLE_H
