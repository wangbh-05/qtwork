//
// Created by gerw on 8/21/24.
//

#include "Map.h"

Map::Map(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath) {}

void Map::scaleToFitScene(QGraphicsScene *scene) {
    // Calculate scale factors to scale the item to fit the entire scene
    QRectF sceneRect = scene->sceneRect();
    QRectF itemRect = boundingRect();

    qreal scaleX = sceneRect.width() / itemRect.width();
    qreal scaleY = sceneRect.height() / itemRect.height();

    // Choose the smaller scale factor to maintain aspect ratio
    qreal scaleFactor = qMin(scaleX, scaleY);

    // Apply the scale to the item
    setTransform(QTransform::fromScale(scaleFactor, scaleFactor), true);

    // Center the item in the scene (optional)
    setPos((sceneRect.width() - boundingRect().width() * scaleFactor) / 2,
           (sceneRect.height() - boundingRect().height() * scaleFactor) / 2);

}

QPointF Map::getSpawnPos() {
    auto currentSceneRect = sceneBoundingRect(); // 使用场景边界矩形
    auto midX = (currentSceneRect.left() + currentSceneRect.right()) * 0.5;
    // 角色出生点在地上方，所以要减去角色高度的一半
    return {midX, getFloorHeight() - boundingRect().height() / 2};
}

qreal Map::getFloorHeight() {
    auto currentSceneRect = sceneBoundingRect(); // 使用场景边界矩形
    return currentSceneRect.top() + (currentSceneRect.top() - currentSceneRect.bottom()) * 0.5;
}

// 新增：实现 getFloorHeightAt，默认返回通用地板高度
qreal Map::getFloorHeightAt(const QPointF& position) {
    Q_UNUSED(position); // 标记参数未使用，消除警告
    return getFloorHeight(); // 默认实现，可以根据需要重写
}

// 新增：实现 getTerrainTypeAt，默认返回 Normal 地形
TerrainType Map::getTerrainTypeAt(const QPointF& position) {
    Q_UNUSED(position); // 标记参数未使用，消除警告
    return Normal; // 默认实现，可以根据需要重写
}
