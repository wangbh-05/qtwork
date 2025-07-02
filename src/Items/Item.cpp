//
// Created by gerw on 8/20/24.
//

#include "Item.h"
#include <QGraphicsObject>
#include <QDebug>

// 【核心修正】 Item 的构造函数现在始终创建 pixmapItem，并初始化 velocity
Item::Item(QGraphicsItem *parent, const QString &pixmapPath) : QGraphicsObject(parent),
    velocity(0,0) // 【修正】明确初始化 QPointF(0,0)，解决歧义
{
    this->pixmapItem = new QGraphicsPixmapItem(this); // 总是创建 pixmapItem，父项设置为 Item 自身
    if (pixmapPath != "") {
        this->pixmapItem->setPixmap(QPixmap(pixmapPath)); // 如果提供了路径，则立即设置图片
    }
    qDebug() << "Item constructor: pixmapItem created for parent" << parent << ". Path provided:" << pixmapPath;
}

// Item 提供的 boundingRect 实现
[[nodiscard]] QRectF Item::boundingRect() const {
    if (pixmapItem != nullptr) {
        return this->pixmapItem->boundingRect();
    }
    return {};
}

// Item 提供的 paint 实现
void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
    // Item 的绘制由其 pixmapItem 自动完成，除非需要额外的绘制
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

// 【新增】Item 的 getVelocity 方法实现
const QPointF &Item::getVelocity() const {
    return velocity;
}

// 【新增】Item 的 setVelocity 方法实现
void Item::setVelocity(const QPointF &velocity) {
    Item::velocity = velocity;
}
