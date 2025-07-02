//
// Created by gerw on 8/21/24.
//

#include "Battlefield.h"

Battlefield::Battlefield(QGraphicsItem *parent) : Map(parent, ":/Items/Maps/Battlefield/g9tOqth.png") {}

qreal Battlefield::getFloorHeight() {
    auto sceneRect = sceneBoundingRect(); // This sceneRect is Battlefield's bounding rect
    return (sceneRect.top() + sceneRect.bottom()) * 0.63; // Battlefield 实际的地板高度
}

// 新增：实现 getFloorHeightAt，返回 Battlefield 实际的地板高度
qreal Battlefield::getFloorHeightAt(const QPointF& position) {
    Q_UNUSED(position); // 对于平坦的战场，地板高度是统一的，所以 position 参数在此处暂时未使用
    return getFloorHeight(); // 直接返回 Battlefield 特定的地板高度
}
