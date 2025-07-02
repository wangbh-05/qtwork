//
// Created by gerw on 8/21/24.
//

#ifndef QT_PROGRAMMING_2024_BATTLEFIELD_H
#define QT_PROGRAMMING_2024_BATTLEFIELD_H


#include "Map.h"

class Battlefield: public Map {
public:
    explicit Battlefield(QGraphicsItem *parent= nullptr);

    qreal getFloorHeight() override; // 重写通用地板高度

    // 新增：重写 getFloorHeightAt，以提供战场特有的地板高度
    qreal getFloorHeightAt(const QPointF& position) override;
};


#endif //QT_PROGRAMMING_2024_BATTLEFIELD_H
