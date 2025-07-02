//
// Created by gerw on 8/21/24.
//

#ifndef QT_PROGRAMMING_2024_MAP_H
#define QT_PROGRAMMING_2024_MAP_H

#include <QGraphicsScene>
#include <QPointF>
#include "../Item.h"

// 将 TerrainType 枚举定义在这里，确保所有包含 Map.h 的文件都能识别它
enum TerrainType {
    Normal,
    Grass,
    Ice,
    // 根据需要添加更多地形类型
};

class Map : public Item {
public:
    explicit Map(QGraphicsItem *parent = nullptr, const QString &pixmapPath = "");

    void scaleToFitScene(QGraphicsScene *scene);

    // 保留原始的 getFloorHeight()，供 Battlefield 重写默认地板高度
    virtual qreal getFloorHeight();

    // 新增：获取特定位置的地板高度 (带参数)
    virtual qreal getFloorHeightAt(const QPointF& position);

    // 新增：获取特定位置的地形类型 (带参数)
    virtual TerrainType getTerrainTypeAt(const QPointF& position);

    virtual QPointF getSpawnPos();
};


#endif //QT_PROGRAMMING_2024_MAP_H
